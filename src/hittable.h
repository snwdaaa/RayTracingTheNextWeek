#ifndef HITTABLE_H
#define HITTABLE_H

#include "Common.h"
#include "AABB.h"

class Material;

// Hit 정보
class HitRecord {
public:
    Point3 p; // 충돌 지점
    Vec3 normal; // 충돌 지점의 법선 벡터
    shared_ptr<Material> mat; // 재질 정보를 담을 포인터
    double t; // 레이 방정식 매개변수
    bool frontFace; // 레이가 바깥쪽에서 들어오는지 여부
    // 텍스처 (u,v) 좌표
    double u;
    double v;

    // outwardNormal은 기존에 구한 법선 벡터
    // outwardNormal은 단위 벡터라고 가정
    void SetFaceNormal(const Ray& r, const Vec3& outwardNormal) {
        frontFace = Dot(r.GetDirection(), outwardNormal) < 0;
        // 항상 레이의 반대 방향으로 설정
        normal = frontFace ? outwardNormal : -outwardNormal; // frontFace면 바깥으로 나가는 방향 그대로, 아니면 반대로
    }
};

// hittable한 오브젝트의 부모가 될 추상 클래스
class Hittable {
public:
    virtual ~Hittable() = default;
    // 레이와 오브젝트의 hit 여부를 판단할 메서드
    virtual bool Hit(const Ray& r, Interval ray_t, HitRecord& rec) const = 0;
    // 오브젝트의 바운딩 박스 리턴하는 메서드
    virtual AABB BoundingBox() const = 0;
    // 물체의 머티리얼 리턴하는 메서드
    virtual shared_ptr<Material> GetMaterial() const = 0;
};

#endif