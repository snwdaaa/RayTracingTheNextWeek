#ifndef TRANSFORM_H
#define TRANSFORM_H

class transform : public hittable {
private:
    shared_ptr<hittable> object;
    matrix4 transform_mat; // 최종 변환 행렬
    matrix4 transform_mat_inv; // 변환 행렬 역행렬
    matrix4 transform_mat_inv_transposed; // 변환 행렬 역행렬의 전치행렬

    // 스케일, 회전, 이동 행렬
    matrix4 scale_mat;
    matrix4 rotate_mat;
    matrix4 translate_mat;

    aabb bbox;
public:
    transform(shared_ptr<hittable> object) : object(object) {
	init_matrix();
	calc_transform_matrix();
	update_bbox();
    }

    transform(shared_ptr<hittable> object, point3 pos) : object(object) {
	init_matrix();
	translate_mat = transformation_matrix::translate(pos.x(), pos.y(), pos.z());
	calc_transform_matrix();
	update_bbox();
    }

    transform(shared_ptr<hittable> object, point3 pos, vec3 rot) 
	: object(object) 
    {
	init_matrix();
	translate_mat = transformation_matrix::translate(pos.x(), pos.y(), pos.z());
	rotate_mat = transformation_matrix::rotate(rot.x(), rot.y(), rot.z());
	calc_transform_matrix();
	update_bbox();
    }

    transform(shared_ptr<hittable> object, point3 pos, vec3 rot, vec3 scale)
	: object(object)
    {
	init_matrix();
	scale_mat = transformation_matrix::scale(scale.x(), scale.y(), scale.z());
	translate_mat = transformation_matrix::translate(pos.x(), pos.y(), pos.z());
	rotate_mat = transformation_matrix::rotate(rot.x(), rot.y(), rot.z());
	calc_transform_matrix();
	update_bbox();
    }

    void init_matrix() {
	// 행렬 초기화
	scale_mat = rotate_mat = translate_mat = identity();
	transform_mat = transform_mat_inv = transform_mat_inv_transposed = identity();
    }

    void calc_transform_matrix()
    {
	// 스케일, 회전, 이동 행렬 계산 후 변환 행렬 계산
	transform_mat = translate_mat * rotate_mat * scale_mat;
	transform_mat_inv = inverse(transform_mat);
	transform_mat_inv_transposed = transpose(transform_mat_inv);
    }

    void update_bbox() {
	// bbox의 8개의 정점을 가져옴 -> 변환 행렬 적용(역행렬 아닌 것 주의)
	// 그 정점으로 bbox 새로 만들어서 업데이트
	auto bbox = object->bounding_box();
	auto x_min = bbox.x.min, x_max = bbox.x.max;
	auto y_min = bbox.y.min, y_max = bbox.y.max;
	auto z_min = bbox.z.min, z_max = bbox.z.max;
	
	std::vector<point3> points;
	points.push_back(point3(x_min, y_max, z_min));
	points.push_back(point3(x_min, y_max, z_max));
	points.push_back(point3(x_max, y_max, z_max));
	points.push_back(point3(x_max, y_max, z_min));
	points.push_back(point3(x_min, y_min, z_min));
	points.push_back(point3(x_min, y_min, z_max));
	points.push_back(point3(x_max, y_min, z_max));
	points.push_back(point3(x_max, y_min, z_min));

	// 각 정점을 변환함과 동시에 x,y,z의 최대/최소 찾기
	double smallest_x = std::numeric_limits<double>::infinity();
	double biggest_x = -std::numeric_limits<double>::infinity();
	double smallest_y = smallest_x;
	double biggest_y = biggest_x;
	double smallest_z = smallest_x;
	double biggest_z = biggest_x;
	for (auto& point : points) {
	    point = to_point3(transform_mat * to_vec4(point));

	    if (point.x() < smallest_x)
		smallest_x = point.x();
	    if (point.y() < smallest_y)
		smallest_y = point.y();
	    if (point.z() < smallest_z)
		smallest_z = point.z();

	    if (point.x() > biggest_x)
		biggest_x = point.x();
	    if (point.y() > biggest_y)
		biggest_y = point.y();
	    if (point.z() > biggest_z)
		biggest_z = point.z();
	}

	auto new_x_interval = interval(smallest_x, biggest_x);
	auto new_y_interval = interval(smallest_y, biggest_y);
	auto new_z_interval = interval(smallest_z, biggest_z);

	bbox = aabb(new_x_interval, new_y_interval, new_z_interval);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
	// 광선 변환
	// 원래라면 물체의 모든 정점에 변환 행렬을 곱해야 하지만
	// 그렇게 한다면 연산량이 너무 많음
	// 대신 광선의 시작점과 방향 벡터에 변환 행렬의 역행렬을 곱해
	// 레이 하나로만 구현할 수 있게 함
	// (역행렬을 곱해 레이를 월드 공간에서 로컬 공간으로 보냄)
	auto transformed_orig = to_point3(to_vec3(
	     transform_mat_inv * to_vec4(r.origin())
	));
	auto transformed_dir = to_vec3(
	    transform_mat_inv * to_vec4(r.direction())
	);

	auto transformed_ray = ray(transformed_orig, transformed_dir, r.time());

	if (!object->hit(transformed_ray, ray_t, rec))
	    return false;

	// 로컬 공간으로 변환된 Ray의 충돌 지점을 다시 월드 공간으로 변환
	rec.p = to_point3(transform_mat * to_vec4(rec.p));

	// 법선 벡터를 일반적인 벡터처럼 변형할 경우
	// 표면에 수직이 아닐 수 있음 (ex: x로만 2배 늘리는 경우)
	// 이를 해결하려면 원래 변환 행렬의 역행렬을 구한 뒤
	// 전치한 행렬을 사용해야 함
	vec3 world_normal = to_vec3(transform_mat_inv_transposed * to_vec4(rec.normal));
	rec.set_face_normal(r, world_normal);

	return true;
    }

    aabb bounding_box() const override { return bbox; }
};

#endif