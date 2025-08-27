#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Common.h"
#include "Hittable.h"
#include "AABB.h"
#include "TransformationMatrix.h"

class Transform : public Hittable {
private:
    shared_ptr<Hittable> object;
    Matrix4 transformMat; // 최종 변환 행렬
    Matrix4 transformMatInv; // 변환 행렬 역행렬
    Matrix4 transformMatInvTransposed; // 변환 행렬 역행렬의 전치행렬

    // 스케일, 회전, 이동 행렬
    Matrix4 scaleMat;
    Matrix4 rotateMat;
    Matrix4 translateMat;

    AABB bbox;
public:
    Transform(shared_ptr<Hittable> object) : object(object) {
		InitMatrix();
		CalcTransformMatrix();
		UpdateBbox();
    }

    Transform(shared_ptr<Hittable> object, Point3 pos) : object(object) {
		InitMatrix();
		translateMat = TransformationMatrix::Translate(pos.x(), pos.y(), pos.z());
		CalcTransformMatrix();
		UpdateBbox();
    }

    Transform(shared_ptr<Hittable> object, Point3 pos, Vec3 rot) 
	: object(object) 
    {
		InitMatrix();
		translateMat = TransformationMatrix::Translate(pos.x(), pos.y(), pos.z());
		rotateMat = TransformationMatrix::Rotate(rot.GetX(), rot.GetY(), rot.GetZ());
		CalcTransformMatrix();
		UpdateBbox();
    }

    Transform(shared_ptr<Hittable> object, Point3 pos, Vec3 rot, Vec3 scale)
	: object(object)
    {
		InitMatrix();
		scaleMat = TransformationMatrix::Scale(scale.GetX(), scale.GetY(), scale.GetZ());
		translateMat = TransformationMatrix::Translate(pos.x(), pos.y(), pos.z());
		rotateMat = TransformationMatrix::Rotate(rot.GetX(), rot.GetY(), rot.GetZ());
		CalcTransformMatrix();
		UpdateBbox();
    }

    void InitMatrix() {
		// 행렬 초기화
		scaleMat = rotateMat = translateMat = GetIdentity();
		transformMat = transformMatInv = transformMatInvTransposed = GetIdentity();
    }

    void CalcTransformMatrix()
    {
		// 스케일, 회전, 이동 행렬 계산 후 변환 행렬 계산
		transformMat = translateMat * rotateMat * scaleMat;
		transformMatInv = GetInverse(transformMat);
		transformMatInvTransposed = GetTranspose(transformMatInv);
    }

    void UpdateBbox() {
		// bbox의 8개의 정점을 가져옴 -> 변환 행렬 적용(역행렬 아닌 것 주의)
		// 그 정점으로 bbox 새로 만들어서 업데이트
		auto objBbox = object->BoundingBox();
		auto xMin = objBbox.x.min, x_max = objBbox.x.max;
		auto yMin = objBbox.y.min, y_max = objBbox.y.max;
		auto zMin = objBbox.z.min, z_max = objBbox.z.max;
	
		std::vector<Point3> points;
		points.push_back(Point3(xMin, y_max, zMin));
		points.push_back(Point3(xMin, y_max, z_max));
		points.push_back(Point3(x_max, y_max, z_max));
		points.push_back(Point3(x_max, y_max, zMin));
		points.push_back(Point3(xMin, yMin, zMin));
		points.push_back(Point3(xMin, yMin, z_max));
		points.push_back(Point3(x_max, yMin, z_max));
		points.push_back(Point3(x_max, yMin, zMin));

		// 각 정점을 변환함과 동시에 x,y,z의 최대/최소 찾기
		double smallestX = std::numeric_limits<double>::infinity();
		double biggestX = -std::numeric_limits<double>::infinity();
		double smallestY = smallestX;
		double biggestY = biggestX;
		double smallestZ = smallestX;
		double biggestZ = biggestX;
		for (auto& point : points) {
			point = ToPoint3(transformMat * ToVec4(point));

			if (point.x() < smallestX)
			smallestX = point.x();
			if (point.y() < smallestY)
			smallestY = point.y();
			if (point.z() < smallestZ)
			smallestZ = point.z();

			if (point.x() > biggestX)
			biggestX = point.x();
			if (point.y() > biggestY)
			biggestY = point.y();
			if (point.z() > biggestZ)
			biggestZ = point.z();
		}

		auto newXInterval = Interval(smallestX, biggestX);
		auto newYInterval = Interval(smallestY, biggestY);
		auto newZInterval = Interval(smallestZ, biggestZ);

		bbox = AABB(newXInterval, newYInterval, newZInterval);
    }

    bool Hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
		// 광선 변환
		// 원래라면 물체의 모든 정점에 변환 행렬을 곱해야 하지만
		// 그렇게 한다면 연산량이 너무 많음
		// 대신 광선의 시작점과 방향 벡터에 변환 행렬의 역행렬을 곱해
		// 레이 하나로만 구현할 수 있게 함
		// (역행렬을 곱해 레이를 월드 공간에서 로컬 공간으로 보냄)
		Vec4 worldOriginVec4 = ToVec4(r.GetOrigin());
		Vec4 localOriginVec4 = transformMatInv * worldOriginVec4;
		Point3 transformedOrig = ToPoint3(localOriginVec4);

		Vec4 worldDirVec4 = ToVec4(r.GetDirection());
		Vec4 localDirVec4 = transformMatInv * worldDirVec4;
		Vec3 transformedDir = ToVec3(localDirVec4);

		auto transformedRay = Ray(transformedOrig, transformedDir, r.GetTime());

		if (!object->Hit(transformedRay, ray_t, rec))
			return false;

		// 로컬 공간으로 변환된 Ray의 충돌 지점을 다시 월드 공간으로 변환
		rec.p = ToPoint3(transformMat * ToVec4(rec.p));

		// 법선 벡터를 일반적인 벡터처럼 변형할 경우
		// 표면에 수직이 아닐 수 있음 (ex: x로만 2배 늘리는 경우)
		// 이를 해결하려면 원래 변환 행렬의 역행렬을 구한 뒤
		// 전치한 행렬을 사용해야 함
		Vec4 localNormalVec4 = ToVec4(rec.normal);
		Vec4 worldNormalVec4 = transformMatInvTransposed * localNormalVec4;
		Vec3 worldNormal = ToVec3(worldNormalVec4);
		rec.SetFaceNormal(r, worldNormal); // 원래 Ray와 앞뒷면 검사

		return true;
    }

    AABB BoundingBox() const override { return bbox; }
};

#endif