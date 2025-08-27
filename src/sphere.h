#ifndef SPHERE_H
#define SPHERE_H

#include "Hittable.h"

// 구 클래스
class Sphere : public Hittable {
private:
    Ray center;
    double radius;
    shared_ptr<Material> mat;
    AABB bbox;

    // 구면 좌표계의 좌표를 uv 좌표계의 좌표로 변환
    static void GetSphereUV(const Point3& p, double& u, double& v) {
        // p: 원점이 중심인 단위 구 위의 한 점
        // u: x=-1부터 y축을 두르면서(가로) 생기는 각 (범위: [0,1])
        // v: y=-1부터 y=1 사이의(세로) 각 (범위: [0,1])
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = std::acos(-p.y());
        auto phi = std::atan2(-p.z(), p.x()) + pi;

        u = phi / (2 * pi);
        v = theta / pi;
    }
public:
    // 정적인 Sphere
    Sphere(double radius, shared_ptr<Material> mat)
        : center(Point3(0, 0, 0), Vec3(0, 0, 0)),
        radius(std::fmax(0, radius)),
        mat(mat)
    {
        auto rvec = Vec3(radius, radius, radius);
        //bbox = AABB(center - rvec, center + rvec);
        auto rayOrigin = center.GetOrigin();
        bbox = AABB(rayOrigin - rvec, rayOrigin + rvec);
    }

    Sphere(const Point3& staticCenter, double radius, shared_ptr<Material> mat) 
        : center(staticCenter, Vec3(0, 0, 0)), 
        radius(std::fmax(0, radius)),
        mat(mat) 
    {
        auto rvec = Vec3(radius, radius, radius);
        //bbox = AABB(center - rvec, center + rvec);
        auto rayOrigin = center.GetOrigin();
        bbox = AABB(rayOrigin - rvec, rayOrigin + rvec);
    }

    // 움직이는 Sphere
    Sphere(const Point3& center1, const Point3& center2, double radius,
        shared_ptr<Material> mat)
        : center(center1, center2 - center1),
        radius(std::fmax(0, radius)),
        mat(mat)
    {
        auto rvec = Vec3(radius, radius, radius);
        bbox = AABB(center1 - rvec, center2 + rvec);
    }

    bool Hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        // 구의 중심을 입력받은 레이가 부딪히는 시점의 시각 값으로 구함
        Point3 currentCenter = center.At(r.GetTime());
        Vec3 oc = currentCenter - r.GetOrigin(); // C-Q
        auto a = r.GetDirection().GetLengthSquared(); // d dot d == |d|^2
        auto h = Dot(r.GetDirection(), oc); // h = d dot (C-Q)
        auto c = oc.GetLengthSquared() - radius * radius; // (C-Q) dot (C-Q) - r^2 = |(C-Q)|^2 - r^2
        auto discriminant = h*h - a*c; // 판별식 h^2 - a*c
    
        if (discriminant < 0) {
            return false;
        }

        auto sqrtd = std::sqrt(discriminant);

        // tmin ~ tmax 사이에서 가장 가까운 교차 지점 찾기
        auto root = (h - sqrtd) / a; // 이차방정식 근의 공식 -> +-라서 실근이 두 개 나오는데, 먼저 -부터 판별
        if (!ray_t.Surrounds(root)) { // -로 판별한 실근이 범위를 벗어나는 경우
            root = (h + sqrtd) / a; // +로 실근 판별
            if (!ray_t.Surrounds(root))
                return false; // +- 둘 다 범위 벗어나는거면 실근 없음 -> 충돌 X
        }

        // 충돌 정보는 HitRecord 객체에 레퍼런스로 전달
        rec.t = root;
        rec.p = r.At(rec.t);
        rec.mat = mat;
        Vec3 outwardNormal = (rec.p - currentCenter) / radius;
        rec.SetFaceNormal(r, outwardNormal); // 법선 벡터 방향 결정
        GetSphereUV(ToPoint3(outwardNormal), rec.u, rec.v);

        return true; // 충돌 O
    }

    // 구 바운딩 박스
    AABB BoundingBox() const override { return bbox; }
};

#endif