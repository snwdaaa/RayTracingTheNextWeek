#ifndef POLYGON_MESH_H
#define POLYGON_MESH_H

class TriangleFace {
public:
    std::vector<int> face;
    AABB bbox;

    TriangleFace(
		std::vector<int>& face,
		AABB bbox
    ) : face(face), bbox(bbox) { }
};

// 단일 폴리곤 메시에 대한 BVH 알고리즘
class MeshBVHNode : public Hittable {
private:
    AABB bbox;
    shared_ptr<Hittable> left;
    shared_ptr<Hittable> right;
    std::vector<Point3>& vertices;
    std::vector<TriangleFace>& faces;

    // 중간 노드는 삼각형과 머티리얼을 가지지 않고 BBOX만 가짐
    bool isLeaf = false;
    TriangleFace& face;
    shared_ptr<Material> mat = NULL;

public:
    MeshBVHNode(
	std::vector<Point3>& vertices,
	std::vector<TriangleFace>& faces,
	const shared_ptr<Material> mat
    ) : MeshBVHNode(vertices, faces, mat, 0, faces.size()) {}

    // BVH 트리 만들기
    MeshBVHNode(
		std::vector<Point3>& vertices, 
		std::vector<TriangleFace>& faces,
		const shared_ptr<Material> mat,
		size_t start,
		size_t end
    ) : vertices(vertices), faces(faces), mat(mat), face(faces[start])
    {
		size_t size = end - start;

		// 재귀 종료 조건 검사
		if (size == 1) {
			// 리프 노드인 경우에만 삼각형과 BBOX를 가짐
			isLeaf = true;
			face = faces[start];
			bbox = face.bbox;
			return;
		}

		// 가장 긴 축 찾기
		// 바운딩 박스의 x, y, z 중 가장 큰 값을 가지는 축이 가장 긴 축
		// 루트 노드에서의 bbox는 쓰레기 값을 가지므로 임시 bbox 설정
		AABB tempBbox = faces[start].bbox;

		// [start, end] 범위에 있는 모든 면을 감싸는 bbox 만듦
		for (size_t i = start + 1; i < end; i++) 
			tempBbox = AABB(tempBbox, faces[i].bbox);

		// 임시 bbox로 가장 긴 축 찾기
		// x(0), y(1), z(2)
		unsigned int longestAxis = tempBbox.GetLongestAxis();

		// 가장 긴 축을 기준으로 면 정렬
		// 정점과 면 정보로 make_triangle_bbox에서 aabb 객체를 만든 후
		// AABB 객체의 GetAxisInterval() 메서드로 가장 긴 축의 interval을 가져옴
		// 그리고 그 interval의 min을 기준으로 정렬
		// 정리하자면, 각 면의 bbox 최소점의 좌표 기준으로 정렬
		auto intervalComp = [&]( // 람다 표현식
			TriangleFace& face_a,
			TriangleFace& face_b
		)
		{
			auto bboxA = face_a.bbox;
			auto bboxB = face_b.bbox;
			// [&]: 외부 변수 참조로 캡처
			auto intervalA = bboxA.GetAxisInterval(longestAxis);
			auto intervalB = bboxB.GetAxisInterval(longestAxis);
			return intervalA.min < intervalB.min;
		};
		// [start, end] 범위만 정렬
		std::sort(faces.begin() + start, faces.begin() + end, intervalComp);

		// 리스트 분할
		size_t mid = start + (size / 2);
		left = make_shared<MeshBVHNode>(vertices, faces, mat, start, mid);
		right = make_shared<MeshBVHNode>(vertices, faces, mat, mid, end);

		// 현재 노드의 bbox 계산
		bbox = AABB(left->BoundingBox(), right->BoundingBox());
    }

    bool Hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
		// 만약 현재 노드가 레이에 부딪히지 않으면
		if (!bbox.Hit(r, ray_t))
			return false;
	
		// 만약 리프 노드인 경우 Ray-Triangle Intersection 판정
		if (isLeaf) {
			Point3 v0 = vertices[face.face[0]];
			Point3 v1 = vertices[face.face[1]];
			Point3 v2 = vertices[face.face[2]];

			// 엣지 벡터 2개
			Vec3 edge1 = v1 - v0;
			Vec3 edge2 = v2 - v0;
			Vec3 P = Cross(r.GetDirection(), edge2); // P = (D x E2)
			double det = Dot(P, edge1); // P dot E1

			// 레이가 삼각형 평면에 평행하다면 바로 false
			// 부동 소수점 오차를 줄이기 위해 epsilon 값 사용
			double epsilon = std::numeric_limits<double>::epsilon();

			// two-sided intersection routine
			//if (-epsilon < det && det < epsilon) 
			//  return false;

			// one-sided intersection routine
			// 정면 삼각형만 렌더링하므로 속도 빠름
			if (det <= epsilon)
			return false;

			// u, v, t 구하기
			// u와 v는 barycentric coordinate이므로 다음 조건을 만족해야 함
			// 0 <= u, v <= 1
			// u + v <= 1

			double invDet = 1.0 / det;
			Vec3 T = r.GetOrigin() - v0;
			double u = invDet * Dot(P, T);

			// u의 유효 범위 검사
			if (u < 0 || u > 1)
			return false;

			Vec3 Q = Cross(T, edge1);
			double v = invDet * Dot(Q, r.GetDirection());

			// v의 유효 범위 검사
			if (v < 0 || u + v > 1)
			return false;

			double t = invDet * Dot(Q, edge2);

			// t의 유효 범위 검사
			// 교차점의 광선이 시작점보다 뒤에 있는 경우는 (t < 0 or t < ray_t.min)
			// 유효한 충돌이 아님
			if (!ray_t.Contains(t))
			return false;

			// rec에 충돌 정보 담아서 리턴
			rec.t = t;
			rec.p = r.At(rec.t);
			rec.mat = mat;

			// 삼각형의 법선 벡터 -> 두 엣지 벡터 외적
			Vec3 outwardNormal = GetUnitVector(Cross(edge1, edge2));
			rec.SetFaceNormal(r, outwardNormal);

			return true;
		}

		// 왼쪽과 오른쪽 노드 or primitive에 충돌 검사
		bool isHitLeft = left->Hit(r, ray_t, rec);
		// 왼쪽 자식에서 교차점을 찾은 경우, 오른쪽 자식 노드에서는
		// 그보다 더 가까운 교차점만 찾음
		auto rightRayT = Interval(ray_t.min, isHitLeft ? rec.t : ray_t.max);
		bool isHitRight = right->Hit(r, rightRayT, rec);

		return isHitLeft || isHitRight;
    }

    AABB BoundingBox() const override {
	return bbox;
    }
};

class PolygonMesh : public Hittable {
private:
    std::string modelPath; // 모델 경로
    std::vector<Point3> vertices; // 정점 정보 배열
    std::vector<TriangleFace> faces; // 면 정보 배열
    std::shared_ptr<Material> mat; // 머티리얼

    // BVH
    AABB bbox;
    shared_ptr<MeshBVHNode> meshBVHRoot;
public:
    PolygonMesh(std::string& modelPath, const shared_ptr<Material> mat) 
	: modelPath(modelPath), mat(mat)
    {
		// 모델 경로 받고 바로 파싱해서 정점과 면 정보를 저장
		ParseObj();

		// bvh 트리 구성
		meshBVHRoot = make_shared<MeshBVHNode>(vertices, faces, mat);

		// BVH 루트의 BBOX == 폴리곤 메시 전체의 BBOX
		bbox = meshBVHRoot->BoundingBox();

		// Scene Info 업데이트
		SceneInfo::vertices += vertices.size();
		SceneInfo::faces += faces.size();
    }
    
    // obj 파일 파싱해서 vertex, face 정보 가져옴
    void ParseObj() {
		// 파일 읽기
		std::ifstream modelFile(modelPath); // 파일 열기

		// 파일 열기 성공 여부 확인
		if (!modelFile.is_open()) {
			std::cerr << "모델 파일 읽기 중 오류 발생\n";
			return;
		}

		std::clog << modelPath << " 모델 불러옴\n";
		std::string line;

		// OBJ 파일에서 한 줄씩 읽어오기
		while (std::getline(modelFile, line)) {
			std::stringstream ss(line); // 문자열을 입력 스트림으로

			// 맨 앞 식별자(v, f) 읽기
			std::string identifier;
			ss >> identifier;

			if (identifier == "v") { // vertex인 경우 vertices에 추가
				double x, y, z;
				ss >> x >> y >> z;
				vertices.push_back(Point3(x, y, z));
			}
			else if (identifier == "f") { // face인 경우 faces에 추가
				int v0_idx, v1_idx, v2_idx;
				ss >> v0_idx >> v1_idx >> v2_idx;
				// obj 파일의 인덱스는 1-based이므로 1을 빼줌
				std::vector<int> face = { v0_idx - 1, v1_idx - 1, v2_idx - 1 };
				faces.push_back(TriangleFace(
					face,
					MakeTriangleBbox(face[0], face[1], face[2]) // BBOX 계산
				));
			}
		}

		modelFile.close(); // 파일 닫기
    }

    bool Hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
		return meshBVHRoot->Hit(r, ray_t, rec);
    }

    AABB BoundingBox() const override { return bbox; }

    // 모든 면의 bbox 미리 계산
    AABB MakeTriangleBbox(const int& v0_idx, const int& v1_idx, const int& v2_idx) {
		// x, y, z 길이 -> 세 정점 각 성분의 min, max -> interval 구하기
		auto x = Interval(
			std::min({
			vertices[v0_idx][0],
			vertices[v1_idx][0],
			vertices[v2_idx][0]
			}),
			std::max({
			vertices[v0_idx][0],
			vertices[v1_idx][0],
			vertices[v2_idx][0]
			})
		);
		auto y = Interval(
			std::min({
			vertices[v0_idx][1],
			vertices[v1_idx][1],
			vertices[v2_idx][1]
			}),
			std::max({
			vertices[v0_idx][1],
			vertices[v1_idx][1],
			vertices[v2_idx][1]
			})
		);
		auto z = Interval(
			std::min({
			vertices[v0_idx][2],
			vertices[v1_idx][2],
			vertices[v2_idx][2]
			}),
			std::max({
			vertices[v0_idx][2],
			vertices[v1_idx][2],
			vertices[v2_idx][2]
			})
		);

		double delta = 0.0001;
		if (x.Size() < delta)
			x = x.Expand(delta);
		if (y.Size() < delta)
			y = y.Expand(delta);
		if (z.Size() < delta)
			z = z.Expand(delta);

		return AABB(x, y, z);
    }
};

#endif