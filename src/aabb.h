#ifndef AABB_H
#define AABB_H

#include "Common.h"

// 자식 노드 또는 primitive를 감싸는 Axis-Aligned Bounding Box
class AABB {
private:
    // Quad는 한 차원의 두께가 0이 될 수 있으므로
    // 두께가 델타보다 작은 경우 padding을 추가
    void PadToMinimums() {
		double delta = 0.0001; // 두께 최소값
		if (x.Size() < delta) x = x.Expand(delta);
		if (y.Size() < delta) y = y.Expand(delta);
		if (z.Size() < delta) z = z.Expand(delta);
    }
public:
    Interval x, y, z; // 각 축의 interval

    AABB() {}

    // 생성자 - 3개의 interval을 받음
    AABB(const Interval& x, const Interval& y, const Interval& z)
	: x(x), y(y), z(z)
    {
		PadToMinimums();
    }

    // 생성자 - 바운딩 박스의 양 끝 점을 받아 각 축의 interval 계산
    AABB(const Point3& a, const Point3& b) {
		x = (a[0] < b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
		y = (a[1] < b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
		z = (a[2] < b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);

		PadToMinimums();
    }

    // 생성자 - 두 바운딩 박스를 모두 포함하는 새로운 바운딩 박스
    AABB(const AABB& bbox1, const AABB& bbox2) {
		x = Interval(bbox1.x, bbox2.x);
		y = Interval(bbox1.y, bbox2.y);
		z = Interval(bbox1.z, bbox2.z);
    }

    // n에 따라 각 축의 Interval 리턴하는 getter 함수
    const Interval& GetAxisInterval(int n) const {
		if (n == 2)
			return z;
		else if (n == 1)
			return y;
		else
			return x;
    }

    // 가장 긴 축의 interval 리턴하는 함수
    const int GetLongestAxis() const {
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
    bool Hit(const Ray& r, Interval ray_t) const {
		// 레이가 각 평면과 만나는 두 지점 t0, t1 찾기
		double t0, t1;
		const Point3& rayStart = r.GetOrigin();
		const Vec3& rayDir = r.GetDirection();

		// x, y, z에 대해 검사
		for (int axis = 0; axis <= 2; axis++) {
			const Interval& axisInterval = GetAxisInterval(axis);
			const double& rayDirAxisInv = 1.0 / rayDir[axis];

			// (x0 - Qx) / dx
			t0 = (axisInterval.min - rayStart[axis]) * rayDirAxisInv;
			// (x1 - Qx) / dx
			t1 = (axisInterval.max - rayStart[axis]) * rayDirAxisInv;

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

AABB operator+(const AABB& bbox, const Vec3& offset) {
    return AABB(bbox.x + offset.GetX(), bbox.y + offset.GetY(), bbox.z + offset.GetZ());
}

AABB operator+(const Vec3& offset, const AABB& bbox) {
    return bbox + offset;
}

#endif