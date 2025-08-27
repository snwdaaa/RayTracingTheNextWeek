// 레이 트레이싱 기본 흐름
// 1. 카메라(eye)에서 화면의 픽셀 통과하는 ray 계산
// 2. 그 ray가 scene의 어떤 오브젝트와 교차하는지 판단
// 3. 가장 가까운 교차 지점에서 색상 계산

// 3D 공간에 있는 가상의 직사각형 화면 viewport
// 렌더링될 이미지의 픽셀 위치와 1:1 매핑됨
// 카메라에서 뷰포트의 각 픽셀의 중심으로 레이를 발사
// 그러면 그 중심에서 똑같은 방향으로 3D 월드 공간에 레이 발사
// 뷰포트 가로/세로 비율 -> 렌더링 이미지의 해상도와 동일해야 픽셀들이 정사각형으로 매핑

// 픽셀 간 간격은 뷰포트의 해상도 크기에 따라 결정
// 대부분 정사각형 픽셀 기준

#include "Hittable.h"
#include "Material.h"

class Camera {
private:
    int imageHeight;	    // 렌더 이미지 높이
    double pixelSamplesScale; // 픽셀 샘플의 누적합에 더할 Color scale factor
    Point3 center;	    // 카메라 센터
    Point3 pixel00Loc;    // (0, 0) 픽셀의 위치
    Vec3 pixelDeltaU;	    // 뷰포트 오른쪽 가리키는 벡터
    Vec3 pixelDeltaV;	    // 뷰포트 아래 가리키는 벡터

    // 임의의 시점(lookfrom)과 바라보는 지점(lookat), 상향 벡터(vup)가 주어지면
    // 카메라는 자신만의 직교 정규 기저(orthonormal basis)인 u, v, w를 가짐
    // w: 카메라의 시선 방향과 반대되는 단위 벡터. lookfrom - lookat으로 구함
    // u: 카메라의 오른쪽을 가리키는 단위 벡터. w와 vup을 외적해 구함
    // v: 카메라의 위쪽을 가리키는 단위 벡터. w와 u를 외적해 구함
    Vec3 u, v, w;

    Vec3 defocusDiskU;    // Defocus 디스크 가로 반지름
    Vec3 defocusDiskV;    // Defocus 디스크 세로 반지름

    void Initialize() {
		// 이미지 높이 계산
		imageHeight = int(imageWidth / aspectRatio);
		imageHeight = (imageHeight < 1) ? 1 : imageHeight; // 높이 1 이상

		pixelSamplesScale = 1.0 / samplesPerPixel;

		// 카메라 속성
		center = lookfrom;
		//auto focal_length = (lookfrom - lookat).length();
		auto theta = DegreesToRadians(vfov);
		// tan(theta/2) = 뷰포트 높이 절반 / focal_length이므로
		// 뷰포트 높이 절반을 구하려면 focal_length를 곱해줘야 함
		// viweport_height는 전체 높이이므로 2를 추가적으로 곱해줌
		auto h = std::tan(theta / 2);
		//auto viewportHeight = 2.0 * h * focal_length;
		auto viewportHeight = 2.0 * h * focusDist;

		// viewportHeight 구할 때 이론적인 aspect ratio가 아닌
		// 이미지의 aspect ratio를 사용
		// truncation때문에 실제 이미지의 aspect ratio와 다를 수 있기 때문
		auto viewportWidth = viewportHeight * (double(imageWidth) / imageHeight);

		w = GetUnitVector(lookfrom - lookat);
		u = GetUnitVector(Cross(vup, w));
		v = Cross(w, u);

		// 뷰포트 엣지 수직, 수평 벡터 계산
		auto viewportU = viewportWidth * u; // 뷰포트 엣지 수평 벡터
		auto viewportV = viewportHeight * -v; // 뷰포트 엣지 수직 벡터(아래로)

		// 픽셀 사이 간격
		pixelDeltaU = viewportU / imageWidth;
		pixelDeltaV = viewportV / imageHeight;

		// 왼쪽 위 픽셀 위치 계산
		// 카메라 센터에서 focal_length만큼 앞으로 가서 뷰포트에 붙은 후
		// 뷰포트 절반만큼 왼쪽으로 & 위쪽으로 이동하면 뷰포트 왼쪽 위에 위치함
		auto viewportUpperLeft = center - (focusDist * w)
			- viewportU / 2 - viewportV / 2;
		// 각 픽셀 중심 -> 뷰포트 왼쪽 위에서 (u + v) 절반만큼 간 위치
		pixel00Loc = viewportUpperLeft
			+ 0.5 * (pixelDeltaU + pixelDeltaV);

		// 카메라 defocus 디스크 가로, 세로 반지름 계산
		auto defocusRadius = focusDist * std::tan(DegreesToRadians(
			defocusAngle / 2));
		defocusDiskU = u * defocusRadius;
		defocusDiskV = v * defocusRadius;
    }

    Color GetRayColor(const Ray& r, int depth, const Hittable& world) const {
		// 최대 depth 이상으로 반사되지 않게 함
		if (depth <= 0)
			return Color(0, 0, 0);

		HitRecord rec;

		// 레이가 아무 물체에도 충돌하지 않으면 배경색 리턴
		if (!world.Hit(r, Interval(0.0001, infinity), rec))
			return background;

		Ray scattered;
		Color attenuation;
		// 방출된 빛
		// 부딪힌 지점의 재질에서 emitted 함수 호출해
		// 물체 자체가 내는 빛의 색을 가져옴
		Color emittedColor = rec.mat->Emit(rec.u, rec.v, rec.p);

		// 만약 물체가 빛을 반사하지 않으면
		// 방출된 빛 그대로 표시
		if (!rec.mat->Scatter(r, rec, attenuation, scattered))
			return emittedColor;

		// 재질이 빛을 반사한다면, 재귀적으로 ray_color 호출해
		// 반사된 광선이 가져오는 빛의 색 계산
		Color scatteredColor = attenuation * GetRayColor(scattered, depth - 1, world);

		// 방출된 빛과 반사된 빛을 더해 최종 색상 결정
		return scatteredColor + emittedColor;
    }

    Ray GetRay(int i, int j) const {
		// 카메라에서 시작해 픽셀 (i, j) 주변의 
		// 랜덤한 샘플 포인트로 향하는 레이 리턴

		// x와 y가 각각 [-0.5, +0.5] 값을 가지는 오프셋 벡터
		auto offset = SampleSquare();
		auto pixelSample = pixel00Loc
			+ ((i + offset.GetX()) * pixelDeltaU)
			+ ((j + offset.GetY()) * pixelDeltaV);
		//auto rayOrigin = center; // 레이 시작은 카메라 센터
		// defocus angle이 0이면 핀홀 방식 -> 항상 선명한 이미지
		// 1 초과하면 레이 시작 지점이 렌즈 디스크 임의의 한 점
		auto rayOrigin = (defocusAngle <= 0) ? center : DefocusDiskSample();
		auto rayDirection = pixelSample - rayOrigin; // 샘플링 지점으로
		auto rayTime = GetRandomDouble(); // [0, 1] 범위 랜덤 시간으로 레이 생성
		return Ray(rayOrigin, rayDirection, rayTime);
    }

    Vec3 SampleSquare() const {
		// [-0.5, +0.5] 범위의 x, y 값을 가지는 벡터 리턴
		return Vec3(GetRandomDouble() - 0.5, GetRandomDouble() - 0.5, 0);
    }

    Point3 DefocusDiskSample() const {
		// 카메라 defocus 디스크에서 랜덤 포인트 리턴
		auto p = RandomInUnitDisk();
		return center + (p[0] * defocusDiskU) + (p[1] * defocusDiskV);
    }
public:
    const std::string outputFilename = "image.ppm";

    double aspectRatio = 16.0 / 9.0; // 종횡비
    int imageWidth = 4096; // 가로 픽셀 개수
    int samplesPerPixel = 10; // 픽셀 당 랜덤 샘플 개수
    int maxDepth = 10; // 레이 반사 재귀호출 최대 depth
    double vfov = 90; // 수직 시야각 (Field of View)
    Color background; // 씬 배경 색상

    Point3 lookfrom; // 카메라의 위치
    Point3 lookat; // 카메라가 바라보는 곳
    Vec3 vup; // 카메라의 위쪽 방향

    // 디스크의 크기를 조절하는 각도
    // 값이 클 수록 조리개가 커져 블러 강해짐
    double defocusAngle = 0;
    double focusDist = 10; // 카메라에서 focus plane까지 거리

    // 렌더 준비 & 렌더 루프 실행
    void Render(const Hittable& world) {
		Initialize(); // 초기화

		std::ofstream out("image.ppm");
		// ppm 파일 헤더 설정
		out << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";

		// 렌더 시간 표시
		std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
		// 이미지를 저장해서 출력할 1차원 벡터
		std::vector<Color>images(imageHeight * imageWidth);

		// 위 -> 아래, 왼쪽 -> 오른쪽으로 그림
		#pragma omp parallel for schedule(dynamic)
		for (int j = 0; j < imageHeight; j++) {
			// 남은 스캔 라인 표시
			std::clog << "\rScanlines remaining: " << (imageHeight - j)
			<< " / " << imageHeight << " " << std::flush;
			for (int i = 0; i < imageWidth; i++) {
				Color pixelColor(0, 0, 0);
				for (int sample = 0; sample < samplesPerPixel; sample++) {
					Ray r = GetRay(i, j); // 픽셀 정사각형 내에서 랜덤 샘플링
					pixelColor += GetRayColor(r, maxDepth, world);
				}
				pixelColor *= pixelSamplesScale; // 평균 구하기
				images[j * imageWidth + i] = pixelColor;
			}
		}

		std::chrono::duration<double>sec = std::chrono::system_clock::now() - start;
		std::cout << "Render time : " << sec.count() << "seconds" << std::endl;

		// images 벡터에 색상 값 다 넣어놓고 한 번에 쓰기
		WriteColor(images, out);

		std::clog << "\rDone                    \n";

		out.close();
		OpenImage(outputFilename); // 이미지 자동 실행
    }
};