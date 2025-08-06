#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"

class quad : public hittable {
private:
    point3 Q; // 시작 지점
    vec3 u, v; // 각 변 방향벡터
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;
    vec3 w;
public:
    quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat)
	: Q(Q), u(u), v(v), mat(mat) 
    {
	// 해당 Quad의 평면 방정식 계산
	// 법선 벡터와 D는 벡터마다 고유함
	auto n = cross(u, v);
	normal = unit_vector(n);
	D = dot(normal, Q);
	w = n / dot(n, n);

	set_bounding_box();
    }

    // 바운딩 박스 계산
    virtual void set_bounding_box() {
	auto bbox_diagonal1 = aabb(Q, Q + u + v); // xy
	auto bbox_diagonal2 = aabb(Q + u, Q + v); // z
	bbox = aabb(bbox_diagonal1, bbox_diagonal2);
    }

    aabb bounding_box() const override { return bbox; };

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
	auto denominator = dot(normal, r.direction()); // t 구하는 식의 분모

	// 레이와 Quad가 평행하면 분모가 0이 됨
	if (std::fabs(denominator) < 1e-8)
	    return false;

	// 레이와 평면의 충돌 지점 t 계산
	auto t = (D - dot(normal, r.origin())) / denominator;
	if (!ray_t.contains(t)) // 레이 범위 검사
	    return false;

	auto intersection = r.at(t); // 레이와 평면의 충돌 지점
	// 평면의 기준점 O와 레이가 충돌한 지점의 방향 벡터
	auto planar_hitpt_vector = intersection - Q;
	// w, 각 변의 방향벡터 uv, 충돌지점 방향벡터로
	// 평면좌표계 좌표 구하기
	auto alpha = dot(w, cross(planar_hitpt_vector, v));
	auto beta = dot(w, cross(u, planar_hitpt_vector));

	if (!is_interior(alpha, beta, rec))
	    return false;

	// 충돌 정보 전달
	rec.t = t;
	rec.p = intersection;
	rec.mat = mat;
	rec.set_face_normal(r, normal);

	return true;
    }

    // Interior Test
    virtual bool is_interior(double a, double b, hit_record& rec) const {
	auto unit_interval = interval(0, 1);

	// 평면좌표계 (alpha, beta)일 때
	// 0 <= alpha <= 1, 0 <= beta <= 1 만족하면 Ray가 Quad와 교차한 것
	if (!unit_interval.contains(a) || !unit_interval.contains(b))
	    return false;

	rec.u = a;
	rec.v = b;
	return true;
    }
};

#endif