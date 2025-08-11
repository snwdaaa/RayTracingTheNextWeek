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
public:
    transform(shared_ptr<hittable> object) : object(object) {
	init_matrix();
	calc_transform_matrix();
    }

    transform(shared_ptr<hittable> object, point3& pos) : object(object) {
	init_matrix();
	translate_mat = transformation_matrix::translate(pos.x(), pos.y(), pos.z());
	calc_transform_matrix();
    }

    transform(shared_ptr<hittable> object, point3& pos, vec3& rot) 
	: object(object) 
    {
	init_matrix();
	translate_mat = transformation_matrix::translate(pos.x(), pos.y(), pos.z());
	rotate_mat = transformation_matrix::rotate(rot.x(), rot.y(), rot.z());
	calc_transform_matrix();
    }

    transform(shared_ptr<hittable> object, point3& pos, vec3& rot, vec3& scale)
	: object(object)
    {
	init_matrix();
	scale_mat = transformation_matrix::scale(scale.x(), scale.y(), scale.z());
	translate_mat = transformation_matrix::translate(pos.x(), pos.y(), pos.z());
	rotate_mat = transformation_matrix::rotate(rot.x(), rot.y(), rot.z());
	calc_transform_matrix();
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

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
	// 광선 변환
	// 원래라면 물체의 모든 정점에 변환 행렬을 곱해야 하지만
	// 그렇게 한다면 연산량이 너무 많음
	// 대신 광선의 시작점과 방향 벡터에 변환 행렬의 역행렬을 곱해
	// 레이 하나로만 구현할 수 있게 함
	// (역행렬을 곱해 레이를 월드 공간에서 로컬 공간으로 보냄)
	auto transformed_orig = to_point3(to_vec3(
	    to_vec4(r.origin()) * transform_mat_inv
	));
	auto transformed_dir = to_vec3(
	    to_vec4(r.direction()) * transform_mat_inv
	);

	auto transformed_ray = ray(transformed_orig, transformed_dir, r.time());

	if (!object->hit(transformed_ray, ray_t, rec))
	    return false;

	// 로컬 공간으로 변환된 Ray의 충돌 지점을 다시 월드 공간으로 변환
	rec.p = to_point3(to_vec4(rec.p) * transform_mat);
	// 법선 벡터를 일반적인 벡터처럼 변형할 경우
	// 표면에 수직이 아닐 수 있음 (ex: x로만 2배 늘리는 경우)
	// 이를 해결하려면 원래 변환 행렬의 역행렬을 구한 뒤
	// 전치한 행렬을 사용해야 함
	rec.normal = to_vec3(to_vec4(rec.normal) * transform_mat_inv_transposed);

	return true;
    }
};

#endif