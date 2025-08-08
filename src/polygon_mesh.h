#ifndef POLYGON_MESH_H
#define POLYGON_MESH_H

class triangle_face {
public:
    std::vector<int> face;
    aabb bbox;

    triangle_face(
	std::vector<int>& face,
	aabb bbox
    ) : face(face), bbox(bbox) { }
};

// 단일 폴리곤 메시에 대한 BVH 알고리즘
class mesh_bvh_node : public hittable {
private:
    aabb bbox;
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    std::vector<point3>& vertices;
    std::vector<triangle_face>& faces;

    // 중간 노드는 삼각형과 머티리얼을 가지지 않고 BBOX만 가짐
    bool isLeaf = false;
    triangle_face& face;
    shared_ptr<material> mat = NULL;

public:
    mesh_bvh_node(
	std::vector<point3>& vertices,
	std::vector<triangle_face>& faces,
	const shared_ptr<material> mat
    ) : mesh_bvh_node(vertices, faces, mat, 0, faces.size()) {}

    // BVH 트리 만들기
    mesh_bvh_node(
	std::vector<point3>& vertices, 
	std::vector<triangle_face>& faces,
	const shared_ptr<material> mat,
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
	aabb temp_bbox = faces[start].bbox;

	// [start, end] 범위에 있는 모든 면을 감싸는 bbox 만듦
	for (size_t i = start + 1; i < end; i++) 
	    temp_bbox = aabb(temp_bbox, faces[i].bbox);

	// 임시 bbox로 가장 긴 축 찾기
	// x(0), y(1), z(2)
	unsigned int longest_axis = temp_bbox.get_longest_axis();

	// 가장 긴 축을 기준으로 면 정렬
	// 정점과 면 정보로 make_triangle_bbox에서 aabb 객체를 만든 후
	// aabb 객체의 get_axis_interval() 메서드로 가장 긴 축의 interval을 가져옴
	// 그리고 그 interval의 min을 기준으로 정렬
	// 정리하자면, 각 면의 bbox 최소점의 좌표 기준으로 정렬
	auto interval_comp = [&]( // 람다 표현식
	    triangle_face& face_a,
	    triangle_face& face_b
	)
	{
	    auto bbox_a = face_a.bbox;
	    auto bbox_b = face_b.bbox;
	    // [&]: 외부 변수 참조로 캡처
	    auto interval_a = bbox_a.get_axis_interval(longest_axis);
	    auto interval_b = bbox_b.get_axis_interval(longest_axis);
	    return interval_a.min < interval_b.min;
	};
	// [start, end] 범위만 정렬
	std::sort(faces.begin() + start, faces.begin() + end, interval_comp);

	// 리스트 분할
	size_t mid = start + (size / 2);
	left = make_shared<mesh_bvh_node>(vertices, faces, mat, start, mid);
	right = make_shared<mesh_bvh_node>(vertices, faces, mat, mid, end);

	// 현재 노드의 bbox 계산
	bbox = aabb(left->bounding_box(), right->bounding_box());
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
	// 만약 현재 노드가 레이에 부딪히지 않으면
	if (!bbox.hit(r, ray_t))
	    return false;
	
	// 만약 리프 노드인 경우 Ray-Triangle Intersection 판정
	if (isLeaf) {
	    point3 v0 = vertices[face.face[0]];
	    point3 v1 = vertices[face.face[1]];
	    point3 v2 = vertices[face.face[2]];

	    // 엣지 벡터 2개
	    vec3 edge1 = v1 - v0;
	    vec3 edge2 = v2 - v0;
	    vec3 P = cross(r.direction(), edge2); // P = (D x E2)
	    double det = dot(P, edge1); // P dot E1

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

	    double inv_det = 1.0 / det;
	    vec3 T = r.origin() - v0;
	    double u = inv_det * dot(P, T);

	    // u의 유효 범위 검사
	    if (u < 0 || u > 1)
		return false;

	    vec3 Q = cross(T, edge1);
	    double v = inv_det * dot(Q, r.direction());

	    // v의 유효 범위 검사
	    if (v < 0 || u + v > 1)
		return false;

	    double t = inv_det * dot(Q, edge2);

	    // t의 유효 범위 검사
	    // 교차점의 광선이 시작점보다 뒤에 있는 경우는 (t < 0 or t < ray_t.min)
	    // 유효한 충돌이 아님
	    if (!ray_t.contains(t))
		return false;

	    // rec에 충돌 정보 담아서 리턴
	    rec.t = t;
	    rec.p = r.at(rec.t);
	    rec.mat = mat;

	    // 삼각형의 법선 벡터 -> 두 엣지 벡터 외적
	    vec3 outward_normal = unit_vector(cross(edge1, edge2));
	    rec.set_face_normal(r, outward_normal);

	    return true;
	}

	// 왼쪽과 오른쪽 노드 or primitive에 충돌 검사
	bool hit_left = left->hit(r, ray_t, rec);
	// 왼쪽 자식에서 교차점을 찾은 경우, 오른쪽 자식 노드에서는
	// 그보다 더 가까운 교차점만 찾음
	auto right_ray_t = interval(ray_t.min, hit_left ? rec.t : ray_t.max);
	bool hit_right = right->hit(r, right_ray_t, rec);

	return hit_left || hit_right;
    }

    aabb bounding_box() const override {
	return bbox;
    }
};

class polygon_mesh : public hittable {
private:
    std::string modelPath; // 모델 경로
    std::vector<point3> vertices; // 정점 정보 배열
    std::vector<triangle_face> faces; // 면 정보 배열
    std::shared_ptr<material> mat; // 머티리얼

    point3 pos; // 위치
    vec3 scale; // 스케일
    // TODO: 회전 기능 추가 -> 4차원 homogeneous matrix

    // BVH
    aabb bbox;
    shared_ptr<mesh_bvh_node> mesh_bvh_root;
public:
    polygon_mesh(
	std::string& modelPath, 
	const shared_ptr<material> mat,
	hittable_list& world, 
	const point3& pos, 
	const vec3& scale
    ) : modelPath(modelPath), mat(mat), pos(pos), scale(scale)
    {
	// 모델 경로 받고 바로 파싱해서 정점과 면 정보를 저장
	parse_obj();

	// bvh 트리 구성
	mesh_bvh_root = make_shared<mesh_bvh_node>(vertices, faces, mat);

	// BVH 루트의 BBOX == 폴리곤 메시 전체의 BBOX
	bbox = mesh_bvh_root->bounding_box();

	// Scene Info 업데이트
	scene_info::vertices += vertices.size();
	scene_info::faces += faces.size();
    }
    
    // obj 파일 파싱해서 vertex, face 정보 가져옴
    void parse_obj() {
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
		vertices.push_back(point3(
		    // 지정된 좌표 값을 더함
		    (x * scale.x()) + pos.x(),
		    (y * scale.y()) + pos.y(), 
		    (z * scale.z()) + pos.z()
		));
	    }
	    else if (identifier == "f") { // face인 경우 faces에 추가
		int v0_idx, v1_idx, v2_idx;
		ss >> v0_idx >> v1_idx >> v2_idx;
		// obj 파일의 인덱스는 1-based이므로 1을 빼줌
		std::vector<int> face = { v0_idx - 1, v1_idx - 1, v2_idx - 1 };
		faces.push_back(triangle_face(
		    face,
		    make_triangle_bbox(face[0], face[1], face[2]) // BBOX 계산
		));
	    }
	}

	modelFile.close(); // 파일 닫기
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
	return mesh_bvh_root->hit(r, ray_t, rec);
    }

    aabb bounding_box() const override {
	return bbox;
    }

    // 모든 면의 bbox 미리 계산
    aabb make_triangle_bbox(const int& v0_idx, const int& v1_idx, const int& v2_idx) {
	// x, y, z 길이 -> 세 정점 각 성분의 min, max -> interval 구하기
	auto x = interval(
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
	auto y = interval(
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
	auto z = interval(
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
	if (x.size() < delta)
	    x = x.expand(delta);
	if (y.size() < delta)
	    y = y.expand(delta);
	if (z.size() < delta)
	    z = z.expand(delta);

	return aabb(x, y, z);
    }
};

#endif