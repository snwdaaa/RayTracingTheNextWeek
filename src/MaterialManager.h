#ifndef MATERIAL_MANAGER_H
#define MATERIAL_MANAGER_H

// 씬 전체에서 사용되는 머티리얼을 관리하는 매니저
class MaterialManager {
private:
	std::unordered_map<std::string, std::pair<shared_ptr<Material>, int>> materials;
	int matIdx = 0;

	shared_ptr<Material> defaultMat; // 예외 처리용 기본 머티리얼
public:
	MaterialManager() {
		defaultMat = make_shared<Lambertian>(Color(0.5, 0.0, 0.5)); // 보라색
	}

	void Add(shared_ptr<Material> mat, std::string name) {
		std::pair<shared_ptr<Material>, int> matPair;
		matPair = std::make_pair(mat, matIdx++);
		materials.insert({ name, matPair });
	}

	shared_ptr<Material> Get(std::string name) {
		std::pair<shared_ptr<Material>, int> matPair;

		auto iter = materials.find(name);
		if (iter != materials.end()) {
			matPair = iter->second;
			return matPair.first;
		}
		else { // Material 못 찾은 경우 기본 머티리얼로
			return defaultMat;
		}
	}
};

#endif