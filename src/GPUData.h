#ifndef GPU_DATA_H
#define GPU_DATA_H

#include "Vec3.h"
#include "Point3.h"
#include "AABB.h"

// GPU에서 Material 구분하는 용도
enum class GPUMaterialType {
	Lambertian,
	Metal,
	Dielectric,
	DiffuseLight,
	Isotropic
};

// 모든 재질의 한 번에 담아 GPU로 전달할 Material 구조체
struct GPUMaterial {
	GPUMaterialType type;
	Color albedo;
	double fuzz = 0.0;
	double refreactionIndex = 0.0;
};

// GPU에서 물체 종류 구분하는 용도
enum class GPUHittableType {
	Sphere,
	Triangle // Quad, PolygonMesh 모두 Triangle로 변환해 처리
};

struct GPUHittable {
	GPUHittableType type;
	int materialIdx; // 이 물체가 사용하는 GPUMaterial 배열 인덱스
	AABB bbox;

	// 기하 정보
	Point3 p0, p1, p2; // Sphere: p0(중심), Triangle: p0, p1, p2(꼭짓점)
	double radius; // Sphere인 경우 반지름
};

#endif