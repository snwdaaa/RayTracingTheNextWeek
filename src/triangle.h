#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Hittable.h"

class Triangle : public Hittable {
private:
    Point3 v0, v1, v2; // 삼각형 점 3개
    shared_ptr<Material> mat;
    AABB bbox;
public:
    Triangle(Point3 v0, Point3 v1, Point3 v2, shared_ptr<Material> mat) 
	: v0(v0), v1(v1), v2(v2), mat(mat) 
    {
		// bbox 계산
		// x, y, z 길이 -> 세 정점 각 성분의 min, max -> interval 구하기
		auto x = Interval(std::min({ v0.x(), v1.x(), v2.x() }),
			std::max({ v0.x(), v1.x(), v2.x() }));
		auto y = Interval(std::min({ v0.y(), v1.y(), v2.y() }),
			std::max({ v0.y(), v1.y(), v2.y() }));
		auto z = Interval(std::min({ v0.z(), v1.z(), v2.z() }),
			std::max({ v0.z(), v1.z(), v2.z() }));

		double delta = 0.0001;
		if (x.Size() < delta)
			x = x.Expand(delta);
		if (y.Size() < delta)
			y = y.Expand(delta);
		if (z.Size() < delta)
			z = z.Expand(delta);

		bbox = AABB(x, y, z);
    }

    bool Hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
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

    AABB BoundingBox() const override { return bbox; }
};

#endif