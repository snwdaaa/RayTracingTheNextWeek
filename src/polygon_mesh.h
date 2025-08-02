#ifndef POLYGON_MESH_H
#define POLYGON_MESH_H

class polygon_mesh {
private:
    std::string modelPath; // 모델 경로
    std::vector<point3> vertices; // 정점 정보 배열
    std::vector<std::vector<int>> faces;
    std::vector<shared_ptr<triangle>> triangles; // 삼각형 면 배열
    std::shared_ptr<material> mat; // 머티리얼
    point3 pos; // 위치
    vec3 scale; // 스케일
public:
    // 생성자 - 모델 경로 받고 바로 파싱해서 정점 정보를 배열에 저장 후
    // hittable_list에 추가
    polygon_mesh(std::string& modelPath, const shared_ptr<material> mat,
	 hittable_list& world, const point3& pos, const vec3& scale)
	: modelPath(modelPath), mat(mat), pos(pos), scale(scale)
    {
	parse_obj();
	make_triangles();
	draw(world);
    }

    // 정점 정보로 삼각형 만들기
    void make_triangles() {
	for (auto face : faces) {
	    triangles.push_back(
		make_shared<triangle>(
		    vertices[face[0]], // v0
		    vertices[face[1]], // v1
		    vertices[face[2]], // v2
		    mat
		)
	    );
	}
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
		//std::clog << "Vertex -> x: " << x + pos.x() << ", y: " << y + pos.y() << ", z: " << z + pos.z() << "\n";
	    }
	    else if (identifier == "f") { // face인 경우 faces에 추가
		int v0_idx, v1_idx, v2_idx;
		ss >> v0_idx >> v1_idx >> v2_idx;
		// obj 파일의 인덱스는 1-based이므로
		// 1을 빼줌
		faces.push_back({ v0_idx-1, v1_idx-1, v2_idx-1 });
		//std::clog << "Face -> v0: " << v0_idx << ", v1: " << v1_idx << ", v2: " << v2_idx << "\n";
	    }
	}

	modelFile.close(); // 파일 닫기
    }

    // hittable_list에 모든 삼각형 추가
    void draw(hittable_list& world) {
	for (auto tri : triangles) {
	    world.add(tri);
	}

	// Scene Info 업데이트
	scene_info::vertices += vertices.size();
	scene_info::faces += triangles.size();
    }
};

#endif