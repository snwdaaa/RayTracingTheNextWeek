#ifndef QUAD_H
#define QUAD_H

#include "Common.h"
#include "Hittable.h"
#include "AABB.h"
#include "HittableList.h"

class Quad : public Hittable {
private:
    Point3 Q; // 시작 지점
    Vec3 u, v; // 각 변 방향벡터
    shared_ptr<Material> mat;
    AABB bbox;
    Vec3 normal;
    double D;
    Vec3 w;
public:
    Quad(const Point3& Q, const Vec3& u, const Vec3& v, shared_ptr<Material> mat)
	: Q(Q), u(u), v(v), mat(mat) 
    {
		// 해당 Quad의 평면 방정식 계산
		// 법선 벡터와 D는 벡터마다 고유함
		auto n = Cross(u, v);
		normal = GetUnitVector(n);
		D = Dot(normal, Q);
		w = n / Dot(n, n);

		SetInitBoundingBox();
    }

    // 바운딩 박스 계산
    virtual void SetInitBoundingBox() {
		auto bboxDiagonal1 = AABB(Q, Q + u + v); // xy
		auto bboxDiagonal2 = AABB(Q + u, Q + v); // z
		bbox = AABB(bboxDiagonal1, bboxDiagonal2);
    }

    AABB BoundingBox() const override { return bbox; };

	shared_ptr<Material> GetMaterial() const override { return mat; }

    bool Hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
		auto denominator = Dot(normal, r.GetDirection()); // t 구하는 식의 분모

		// 레이와 Quad가 평행하면 분모가 0이 됨
		if (std::fabs(denominator) < std::numeric_limits<double>::epsilon())
			return false;

		// 레이와 평면의 충돌 지점 t 계산
		auto t = (D - Dot(normal, r.GetOrigin())) / denominator;
		if (!ray_t.Contains(t)) // 레이 범위 검사
			return false;

		auto intersection = r.At(t); // 레이와 평면의 충돌 지점
		// 평면의 기준점 O와 레이가 충돌한 지점의 방향 벡터
		auto planarHitPointVector = intersection - Q;
		// w, 각 변의 방향벡터 uv, 충돌지점 방향벡터로
		// 평면좌표계 좌표 구하기
		auto alpha = Dot(w, Cross(planarHitPointVector, v));
		auto beta = Dot(w, Cross(u, planarHitPointVector));

		if (!IsInterior(alpha, beta, rec))
			return false;

		// 충돌 정보 전달
		rec.t = t;
		rec.p = intersection;
		rec.mat = mat;
		rec.SetFaceNormal(r, normal);

		return true;
    }

    // Interior Test
    virtual bool IsInterior(double a, double b, HitRecord& rec) const {
		auto unitInterval = Interval(0, 1);

		// 평면좌표계 (alpha, beta)일 때
		// 0 <= alpha <= 1, 0 <= beta <= 1 만족하면 Ray가 Quad와 교차한 것
		if (!unitInterval.Contains(a) || !unitInterval.Contains(b))
			return false;

		rec.u = a;
		rec.v = b;
		return true;
    }
};

inline shared_ptr<Hittable> MakeBox(const Point3& a, const Point3& b, shared_ptr<Material> mat) {
    // 양 끝점 a와 b로 만들어지는 3D 큐브 리턴
    
    auto sides = make_shared<HittableList>();

    // 양 끝 정점 구하기
    auto min = Point3(
	std::fmin(a.x(), b.x()),
	std::fmin(a.y(), b.y()),
	std::fmin(a.z(), b.z())
    );
    auto max = Point3(
	std::fmax(a.x(), b.x()),
	std::fmax(a.y(), b.y()),
	std::fmax(a.z(), b.z())
    );

    auto dx = Vec3(max.x() - min.x(), 0, 0);
    auto dy = Vec3(0, max.y() - min.y(), 0);
    auto dz = Vec3(0, 0, max.z() - min.z());

    sides->Add(make_shared<Quad>(Point3(min.x(), min.y(), max.z()), dx, dy, mat)); // front
    sides->Add(make_shared<Quad>(Point3(max.x(), min.y(), max.z()), -dz, dy, mat)); // right
    sides->Add(make_shared<Quad>(Point3(max.x(), min.y(), min.z()), -dx, dy, mat)); // back
    sides->Add(make_shared<Quad>(Point3(min.x(), min.y(), min.z()), dz, dy, mat)); // left
    sides->Add(make_shared<Quad>(Point3(min.x(), max.y(), max.z()), dx, -dz, mat)); // top
    sides->Add(make_shared<Quad>(Point3(min.x(), min.y(), min.z()), dx, dz, mat)); // bottom

    return sides;
}

#endif