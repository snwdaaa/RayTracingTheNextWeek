﻿#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

// 구 클래스
class sphere : public hittable {
private:
    ray center;
    double radius;
    shared_ptr<material> mat;
    aabb bbox;
public:
    // 정적인 Sphere
    sphere(const point3& static_center, double radius, shared_ptr<material> mat) 
        : center(static_center, vec3(0, 0, 0)), 
        radius(std::fmax(0, radius)),
        mat(mat) 
    {
        auto rvec = vec3(radius, radius, radius);
        //bbox = aabb(center - rvec, center + rvec);
        auto ray_origin = center.origin();
        bbox = aabb(ray_origin - rvec, ray_origin + rvec);
    }

    // 움직이는 Sphere
    sphere(const point3& center1, const point3& center2, double radius,
        shared_ptr<material> mat)
        : center(center1, center2 - center1),
        radius(std::fmax(0, radius)),
        mat(mat)
    {
        auto rvec = vec3(radius, radius, radius);
        bbox = aabb(center1 - rvec, center2 + rvec);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // 구의 중심을 입력받은 레이가 부딪히는 시점의 시각 값으로 구함
        point3 current_center = center.at(r.time());
        vec3 oc = current_center - r.origin(); // C-Q
        auto a = r.direction().length_squared(); // d dot d == |d|^2
        auto h = dot(r.direction(), oc); // h = d dot (C-Q)
        auto c = oc.length_squared() - radius * radius; // (C-Q) dot (C-Q) - r^2 = |(C-Q)|^2 - r^2
        auto discriminant = h*h - a*c; // 판별식 h^2 - a*c
    
        if (discriminant < 0) {
            return false;
        }

        auto sqrtd = std::sqrt(discriminant);

        // tmin ~ tmax 사이에서 가장 가까운 교차 지점 찾기
        auto root = (h - sqrtd) / a; // 이차방정식 근의 공식 -> +-라서 실근이 두 개 나오는데, 먼저 -부터 판별
        if (!ray_t.surrounds(root)) { // -로 판별한 실근이 범위를 벗어나는 경우
            root = (h + sqrtd) / a; // +로 실근 판별
            if (!ray_t.surrounds(root))
                return false; // +- 둘 다 범위 벗어나는거면 실근 없음 -> 충돌 X
        }

        // 충돌 정보는 hit_record 객체에 레퍼런스로 전달
        rec.t = root;
        rec.p = r.at(rec.t);
        rec.mat = mat;
        vec3 outward_normal = (rec.p - current_center) / radius;
        rec.set_face_normal(r, outward_normal); // 법선 벡터 방향 결정

        return true; // 충돌 O
    }

    // 구 바운딩 박스
    aabb bounding_box() const override {
        return bbox;
    }
};

#endif