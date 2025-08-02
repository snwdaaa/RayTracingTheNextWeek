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

#include "hittable.h"
#include "material.h"

class camera {
private:
    int image_height;	    // 렌더 이미지 높이
    double pixel_samples_scale; // 픽셀 샘플의 누적합에 더할 Color scale factor
    point3 center;	    // 카메라 센터
    point3 pixel00_loc;    // (0, 0) 픽셀의 위치
    vec3 pixel_delta_u;	    // 뷰포트 오른쪽 가리키는 벡터
    vec3 pixel_delta_v;	    // 뷰포트 아래 가리키는 벡터

    // 임의의 시점(lookfrom)과 바라보는 지점(lookat), 상향 벡터(vup)가 주어지면
    // 카메라는 자신만의 직교 정규 기저(orthonormal basis)인 u, v, w를 가짐
    // w: 카메라의 시선 방향과 반대되는 단위 벡터. lookfrom - lookat으로 구함
    // u: 카메라의 오른쪽을 가리키는 단위 벡터. w와 vup을 외적해 구함
    // v: 카메라의 위쪽을 가리키는 단위 벡터. w와 u를 외적해 구함
    vec3 u, v, w;

    vec3 defocus_disk_u;    // Defocus 디스크 가로 반지름
    vec3 defocus_disk_v;    // Defocus 디스크 세로 반지름

    void initialize() {
	// 이미지 높이 계산
	image_height = int(image_width / aspect_ratio);
	image_height = (image_height < 1) ? 1 : image_height; // 높이 1 이상

	pixel_samples_scale = 1.0 / samples_per_pixel;

	// 카메라 속성
	center = lookfrom;
	//auto focal_length = (lookfrom - lookat).length();
	auto theta = degrees_to_radians(vfov);
	// tan(theta/2) = 뷰포트 높이 절반 / focal_length이므로
	// 뷰포트 높이 절반을 구하려면 focal_length를 곱해줘야 함
	// viweport_height는 전체 높이이므로 2를 추가적으로 곱해줌
	auto h = std::tan(theta / 2);
	//auto viewport_height = 2.0 * h * focal_length;
	auto viewport_height = 2.0 * h * focus_dist;

	// viewport_height 구할 때 이론적인 aspect ratio가 아닌
	// 이미지의 aspect ratio를 사용
	// truncation때문에 실제 이미지의 aspect ratio와 다를 수 있기 때문
	auto viewport_width = viewport_height * (double(image_width) / image_height);

	w = unit_vector(lookfrom - lookat);
	u = unit_vector(cross(vup, w));
	v = cross(w, u);

	// 뷰포트 엣지 수직, 수평 벡터 계산
	auto viewport_u = viewport_width * u; // 뷰포트 엣지 수평 벡터
	auto viewport_v = viewport_height * -v; // 뷰포트 엣지 수직 벡터(아래로)

	// 픽셀 사이 간격
	pixel_delta_u = viewport_u / image_width;
	pixel_delta_v = viewport_v / image_height;

	// 왼쪽 위 픽셀 위치 계산
	// 카메라 센터에서 focal_length만큼 앞으로 가서 뷰포트에 붙은 후
	// 뷰포트 절반만큼 왼쪽으로 & 위쪽으로 이동하면 뷰포트 왼쪽 위에 위치함
	auto viewport_upper_left = center - (focus_dist * w)
	    - viewport_u / 2 - viewport_v / 2;
	// 각 픽셀 중심 -> 뷰포트 왼쪽 위에서 (u + v) 절반만큼 간 위치
	pixel00_loc = viewport_upper_left
	    + 0.5 * (pixel_delta_u + pixel_delta_v);

	// 카메라 defocus 디스크 가로, 세로 반지름 계산
	auto defocus_radius = focus_dist * std::tan(degrees_to_radians(
	    defocus_angle / 2));
	defocus_disk_u = u * defocus_radius;
	defocus_disk_v = v * defocus_radius;
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
	// 최대 depth 이상으로 반사되지 않게 함
	if (depth <= 0)
	    return color(0, 0, 0);

	hit_record rec;

	// 물체에 충돌한 경우
	if (world.hit(r, interval(0.0001, infinity), rec)) {
	    ray scattered;
	    color attenuation;
	    if (rec.mat->scatter(r, rec, attenuation, scattered)) {
		return attenuation * ray_color(scattered, depth - 1, world);
	    }
	    return color(0, 0, 0);
	}

	// 물체에 충돌하지 않은 경우 배경색 그림
	vec3 unit_direction = unit_vector(r.direction());
	auto a = 0.5 * (unit_direction.y() + 1.0);
	// lerp
	// a가 1이면 하늘색 (0.5, 0.7, 1.0), 0이면 흰색, 그 사이는 blend
	return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

    ray get_ray(int i, int j) const {
	// 카메라에서 시작해 픽셀 (i, j) 주변의 
	// 랜덤한 샘플 포인트로 향하는 레이 리턴

	// x와 y가 각각 [-0.5, +0.5] 값을 가지는 오프셋 벡터
	auto offset = sample_square();
	auto pixel_sample = pixel00_loc
	    + ((i + offset.x()) * pixel_delta_u)
	    + ((j + offset.y()) * pixel_delta_v);
	//auto ray_origin = center; // 레이 시작은 카메라 센터
	// defocus angle이 0이면 핀홀 방식 -> 항상 선명한 이미지
	// 1 초과하면 레이 시작 지점이 렌즈 디스크 임의의 한 점
	auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
	auto ray_direction = pixel_sample - ray_origin; // 샘플링 지점으로
	return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
	// [-0.5, +0.5] 범위의 x, y 값을 가지는 벡터 리턴
	return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 defocus_disk_sample() const {
	// 카메라 defocus 디스크에서 랜덤 포인트 리턴
	auto p = random_in_unit_disk();
	return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
public:
    const std::string outputFilename = "image.ppm";

    double aspect_ratio = 16.0 / 9.0; // 종횡비
    int image_width = 4096; // 가로 픽셀 개수
    int samples_per_pixel = 10; // 픽셀 당 랜덤 샘플 개수
    int max_depth = 10; // 레이 반사 재귀호출 최대 depth
    double vfov = 90; // 수직 시야각 (Field of View)

    point3 lookfrom; // 카메라의 위치
    point3 lookat; // 카메라가 바라보는 곳
    vec3 vup; // 카메라의 위쪽 방향

    // 디스크의 크기를 조절하는 각도
    // 값이 클 수록 조리개가 커져 블러 강해짐
    double defocus_angle = 0;
    double focus_dist = 10; // 카메라에서 focus plane까지 거리

    // 렌더 준비 & 렌더 루프 실행
    void render(const hittable& world) {
	initialize(); // 초기화

	std::ofstream out("image.ppm");
	// ppm 파일 헤더 설정
	out << "P3\n" << image_width << " " << image_height << "\n255\n";

	// 렌더 시간 표시
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	// 이미지를 저장해서 출력할 1차원 벡터
	std::vector<color>images(image_height * image_width);

	// 위 -> 아래, 왼쪽 -> 오른쪽으로 그림
	#pragma omp parallel for schedule(dynamic)
	for (int j = 0; j < image_height; j++) {
	    // 남은 스캔 라인 표시
	    std::clog << "\rScanlines remaining: " << (image_height - j)
		<< " / " << image_height << " " << std::flush;
	    for (int i = 0; i < image_width; i++) {
		color pixel_color(0, 0, 0);
		for (int sample = 0; sample < samples_per_pixel; sample++) {
		    ray r = get_ray(i, j); // 픽셀 정사각형 내에서 랜덤 샘플링
		    pixel_color += ray_color(r, max_depth, world);
		}
		pixel_color *= pixel_samples_scale; // 평균 구하기
		images[j * image_width + i] = pixel_color;
	    }
	}

	std::chrono::duration<double>sec = std::chrono::system_clock::now() - start;
	std::cout << "Render time : " << sec.count() << "seconds" << std::endl;

	// images 벡터에 색상 값 다 넣어놓고 한 번에 쓰기
	write_color(images, out);

	std::clog << "\rDone                    \n";

	out.close();
	openImage(outputFilename); // 이미지 자동 실행
    }
};