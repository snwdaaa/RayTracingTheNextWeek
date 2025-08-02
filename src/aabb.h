#ifndef AABB_H
#define AABB_H

// 자식 노드 또는 primitive를 감싸는 Axis-Aligned Bounding Box
class aabb {
private:
    interval x, y, z; // 각 축의 interval

public:
    aabb() {}

    // 생성자 - 3개의 interval을 받음
    aabb(const interval& x, const interval& y, const interval& z)
	: x(x), y(y), z(z) { }

    // 생성자 - 바운딩 박스의 양 끝 점을 받아 각 축의 interval 계산
    aabb(const point3& a, const point3& b) {
	x = (a[0] < b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
	y = (a[1] < b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
	z = (a[2] < b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
    }

    // 생성자 - 두 바운딩 박스를 모두 포함하는 새로운 바운딩 박스
    aabb(const aabb& bbox1, const aabb& bbox2) {
	x = interval(bbox1.x, bbox2.x);
	y = interval(bbox1.y, bbox2.y);
	z = interval(bbox1.z, bbox2.z);
    }

    // n에 따라 각 축의 interval 리턴하는 getter 함수
    const interval& get_axis_interval(int n) const {
	if (n == 2)
	    return z;
	else if (n == 1)
	    return y;
	else
	    return x;
    }

    // 가장 긴 축의 interval 리턴하는 함수
    const int get_longest_axis() const {
	auto length_x = x.max - x.min;
	auto length_y = y.max - y.min;
	auto length_z = z.max - z.min;
	if (length_x > length_y) {
	    if (length_x > length_z)
		return 0;
	    else
		return 2;
	}
	else if (length_y > length_z)
	    return 1;
	else 
	    return 2;
    }

    // ray가 각 축의 slab에 모두 겹치는지 확인하는 hit 함수
    bool hit(const ray& r, interval ray_t) const {
	// 레이가 각 평면과 만나는 두 지점 t0, t1 찾기
	double t0, t1;
	const point3& ray_start = r.origin();
	const vec3& ray_dir = r.direction();

	// x, y, z에 대해 검사
	for (int axis = 0; axis <= 2; axis++) {
	    const interval& axis_interval = get_axis_interval(axis);
	    const double& ray_dir_axis_inv = 1.0 / ray_dir[axis];

	    // (x0 - Qx) / dx
	    t0 = (axis_interval.min - ray_start[axis]) * ray_dir_axis_inv;
	    // (x1 - Qx) / dx
	    t1 = (axis_interval.max - ray_start[axis]) * ray_dir_axis_inv;

	    // ray_t는 지금까지 검사해온 bbox의 t가 담겨있음
	    // 만약 ray_t의 min보다 방금 검사한 t0가 더 크다면
	    // 더 안쪽에 있는 bbox에 맞은 것이므로 ray_t 업데이트
	    // 만약 ray_t의 max보다 방금 검사한 t1이 더 작다면
	    // 더 안쪽에 있는 bbox에 맞은 것이므로 ray_t 업데이트
	    if (t0 < t1) { // 레이가 + 방향으로 진행
		ray_t.min = std::max(ray_t.min, t0);
		ray_t.max = std::min(ray_t.max, t1);
	    }
	    else { // 레이가 - 방향으로 진행 -> 반대로 해주면 됨
		ray_t.min = std::max(ray_t.min, t1);
		ray_t.max = std::min(ray_t.max, t0);
	    }

	    // 단 하나의 축이라도 최종 이탈 시각이 최종 진입 시각보다 작으면
	    // 세 slab에 동시에 머무르지 않으므로 충돌하지 않은 것
	    if (ray_t.max <= ray_t.min) 
		return false;
	}

	return true;
    }
};

#endif