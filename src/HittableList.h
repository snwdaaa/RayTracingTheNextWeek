#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "Hittable.h"

#include <vector>

// Hittable 오브젝트를 담는 클래스
// hittable을 상속받아서 Hit 메서드를 통해 담긴 모든 hittable 오브젝트에 대한 hit 검사를 할 수 있게 함
class HittableList : public Hittable {
private:
    AABB bbox; // 씬 전체를 포함하는 바운딩 박스
public:
    std::vector<shared_ptr<Hittable>> objects;
        
    HittableList() {}
    HittableList(shared_ptr<Hittable> object) { Add(object); }

    void Clear() { objects.clear(); }

    void Add(shared_ptr<Hittable> object) {
        objects.push_back(object);
        // 새로운 오브젝트가 들어오면 그걸 포함해 다시 씬 전체 bbox 계산
        bbox = AABB(bbox, object->BoundingBox());
    }

    // 벡터에 저장된 모든 hittable에 대해 순차적으로 hit 검사
    // rec에는 가장 가까운 충돌 정보가 저장돼 리턴
    bool Hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
        HitRecord tempRec;
        bool hitAnything = false;
        auto closestSoFar = ray_t.max; // 가장 가까운 충돌만 기록하기 위한 변수

        for (const auto& object : objects) {
            if (object->Hit(r, Interval(ray_t.min, closestSoFar), tempRec)) { // 오브젝트가 레이에 부딪히면
                hitAnything = true; // 하나라도 충돌이 있으면 true
                closestSoFar = tempRec.t;
                rec = tempRec;
            }
        }

        return hitAnything;
    }

    AABB BoundingBox() const override{
        return bbox;
    }
};

#endif