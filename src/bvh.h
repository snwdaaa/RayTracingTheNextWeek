#ifndef BVH_H
#define BVH_H

class bvh_node : public hittable {
private:
    // 자식 노드 또는 primitive가 모두 포함된 bbox
    // 중간 노드인 경우 두 자식 노드의 bbox를 모두 포함하는 bbox
    // 리프 노드인 경우 primitive를 포함하는 bbox
    aabb bbox;
    // 왼쪽, 오른쪽 자식 노드 가리키는 포인터
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;

public:
    // hittable_list를 implicit하게 복사하는 생성자
    bvh_node(hittable_list list) 
	: bvh_node(list.objects, 0, list.objects.size()) 
    {
    }

    bvh_node(std::vector<shared_ptr<hittable>>& objects,
	size_t start, size_t end) 
    {
	// ---------------------------------------------------------------------
	// BVH 볼륨 분할의 핵심
	// 물체들을 공간적으로 잘 분리하여, 생성될 두 자식 노드의 bbox가
	// 최대한 겹치지 않고 작게 만들어지도록 하는 것
	// 이를 위해 하나의 축을 정해 그 축을 기준으로 정렬한 뒤, 리스트를 반으로 나눔

	// 분할은 BVH의 생성자에서 구현
	// 1. 축 선택하기
	// 어느 축을 기준으로 나눌지 결정
	// X, Y, Z 축 중 가장 긴 축을 선택하는 것이 일반적
	// 2. 객체 정렬 (할 필요 없다고는 함)
	// 선택된 축을 기준으로, 현재 노드에 속한 모든 물체들을 정렬
	// 정렬 기준 -> 각 bbox의 interval min값
	// 3. 리스트 분할
	// 정렬된 물체 리스트 정확히 절반으로 나눔
	// 앞쪽 절반 물체 -> 왼쪽 자식 노드 구성하는데 사용
	// 뒤쪽 절반 물체 -> 오른쪽 자식 노드 구성하는데 사용
	// 이 두 하위 리스트 가지고 bvh_node 생성자 재귀적으로 호출해 트리 계속 만듦

	// 재귀 종료 조건
	// 1. 물체가 1개일 때 -> 더 이상 나눌 수 없으므로, 이 노드는 리프 노드
	// left, right가 모두 자기 자신을 가리키도록 함
	// 2. 물체가 2개일 때 -> 정렬할 필요 x, 하나는 왼쪽 자식, 다른 하나는
	// 오른쪽 자식으로 설정하고 재귀 종료
	// ---------------------------------------------------------------------

	size_t size = end - start;

	// 재귀 종료 조건 검사
	if (size == 1) {
	    left = right = objects[start];
	    bbox = left->bounding_box();
	    return;
	}
	else if (size == 2) {
	    left = objects[start];
	    right = objects[start + 1];
	    bbox = aabb(left->bounding_box(), right->bounding_box());
	    return;
	}

	// 가장 긴 축 찾기
	// 바운딩 박스의 x, y, z 중 가장 큰 값을 가지는 축이 가장 긴 축
	// 루트 노드에서의 bbox는 쓰레기 값을 가지므로 임시 bbox 설정
	aabb temp_bbox = objects[start]->bounding_box();

	// [start, end] 범위에 있는 모든 오브젝트를 감싸는 bbox 만듦
	for (size_t i = start + 1; i < end; i++)
	    temp_bbox = aabb(temp_bbox, objects[i]->bounding_box());

	// 임시 bbox로 가장 긴 축 찾기
	// x(0), y(1), z(2)
	unsigned int longest_axis = temp_bbox.get_longest_axis();

	// 가장 긴 축을 기준으로 오브젝트 정렬
	// hittable 객체의 bounding_box() 메서드로 aabb 객체를 가져온 후
	// aabb 객체의 get_axis_interval() 메서드로 가장 긴 축의 interval을 가져옴
	// 그리고 그 interval의 min을 기준으로 정렬
	auto interval_comp = [&]( // 람다 표현식
	    shared_ptr<hittable> obj_a,
	    shared_ptr<hittable> obj_b
	)
	{
	    auto bbox_a = obj_a->bounding_box();
	    auto bbox_b = obj_b->bounding_box();
	    // [&]: 외부 변수 참조로 캡처
	    auto interval_a = bbox_a.get_axis_interval(longest_axis);
	    auto interval_b = bbox_b.get_axis_interval(longest_axis);
	    return interval_a.min < interval_b.min;
	};
	// [start, end] 범위만 정렬
	std::sort(objects.begin() + start, objects.begin() + end, interval_comp);

	// 리스트 분할
	size_t mid = start + (size / 2); // 전체 object에서의 절대적 위치
	left = make_shared<bvh_node>(objects, start, mid);
	right = make_shared<bvh_node>(objects, mid, end);

	// 현재 노드의 bbox 계산
	bbox = aabb(left->bounding_box(), right->bounding_box());
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
	// 만약 현재 노드가 레이에 부딪히지 않으면
	if (!bbox.hit(r, ray_t))
	    return false;

	// 왼쪽과 오른쪽 노드 or primitive에 충돌 검사
	bool hit_left = left->hit(r, ray_t, rec);
	// 왼쪽 자식에서 교차점을 찾은 경우, 오른쪽 자식 노드에서는
	// 그보다 더 가까운 교차점만 찾음
	auto right_ray_t = interval(ray_t.min, hit_left ? rec.t : ray_t.max);
	bool hit_right = right->hit(r, right_ray_t, rec);

	return hit_left || hit_right; // 둘 중 하나라도 hit 하는 경우에만 true
    }

    aabb bounding_box() const {
	return bbox;
    }
};

#endif