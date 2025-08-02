#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"

class triangle : public hittable {
private:
    point3 v0, v1, v2; // 삼각형 점 3개
    shared_ptr<material> mat;
    aabb bbox;
public:
    triangle(point3 v0, point3 v1, point3 v2, shared_ptr<material> mat) 
	: v0(v0), v1(v1), v2(v2), mat(mat) 
    {
	// bbox 계산
	// x, y, z 길이 -> 세 정점 각 성분의 min, max -> interval 구하기
	auto x = interval(std::min({ v0.x(), v1.x(), v2.x() }),
	    std::max({ v0.x(), v1.x(), v2.x() }));
	auto y = interval(std::min({ v0.y(), v1.y(), v2.y() }),
	    std::max({ v0.y(), v1.y(), v2.y() }));
	auto z = interval(std::min({ v0.z(), v1.z(), v2.z() }),
	    std::max({ v0.z(), v1.z(), v2.z() }));

	double delta = 0.0001;
	if (x.size() < delta)
	    x = x.expand(delta);
	if (y.size() < delta)
	    y = y.expand(delta);
	if (z.size() < delta)
	    z = z.expand(delta);

	bbox = aabb(x, y, z);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
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

    aabb bounding_box() const override {
	return bbox;
    }
};

#endif