#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

// 변환 행렬 생성 클래스
class transformation_matrix {
public:
    static matrix4 translate(double tx, double ty, double tz) {
	auto id_matrix = identity();

	id_matrix[0][3] = tx;
	id_matrix[1][3] = ty;
	id_matrix[2][3] = tz;

	return id_matrix;
    }

    static matrix4 rotate_x(double deg) {
	auto rad = degrees_to_radians(deg);

	auto matrix = identity();

	matrix[1][1] = cos(rad);
	matrix[1][2] = -sin(rad);
	matrix[2][1] = sin(rad);
	matrix[2][2] = cos(rad);

	return matrix;
    }

    static matrix4 rotate_y(double deg) {
	auto rad = degrees_to_radians(deg);

	auto matrix = identity();

	matrix[0][0] = cos(rad);
	matrix[0][2] = sin(rad);
	matrix[2][0] = -sin(rad);
	matrix[2][2] = cos(rad);

	return matrix;
    }

    static matrix4 rotate_z(double deg) {
	auto rad = degrees_to_radians(deg);

	auto matrix = identity();

	matrix[0][0] = cos(rad);
	matrix[0][1] = -sin(rad);
	matrix[1][0] = sin(rad);
	matrix[1][1] = cos(rad);

	return matrix;
    }

    static matrix4 rotate(double dx, double dy, double dz) {
	return rotate_x(dx) * rotate_y(dy) * rotate_z(dz);
    }

    static matrix4 scale(double sx, double sy, double sz) {
	auto matrix = identity();

	matrix[0][0] = sx;
	matrix[1][1] = sy;
	matrix[2][2] = sz;

	return matrix;
    }
};



#endif TRANSFORMATION_H