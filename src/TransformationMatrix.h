#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "Common.h"

// 변환 행렬 생성 클래스
class TransformationMatrix {
public:
    static Matrix4 Translate(double tx, double ty, double tz) {
		auto matrix = GetIdentity();

		matrix[0][3] = tx;
		matrix[1][3] = ty;
		matrix[2][3] = tz;

		return matrix;
    }

    static Matrix4 RotateX(double deg) {
		auto rad = DegreesToRadians(deg);
		auto matrix = GetIdentity();

		matrix[1][1] = cos(rad);
		matrix[1][2] = -sin(rad);
		matrix[2][1] = sin(rad);
		matrix[2][2] = cos(rad);

		return matrix;
    }

    static Matrix4 RotateY(double deg) {
		auto rad = DegreesToRadians(deg);
		auto matrix = GetIdentity();

		matrix[0][0] = cos(rad);
		matrix[0][2] = sin(rad);
		matrix[2][0] = -sin(rad);
		matrix[2][2] = cos(rad);

		return matrix;
    }

    static Matrix4 RotateZ(double deg) {
		auto rad = DegreesToRadians(deg);
		auto matrix = GetIdentity();

		matrix[0][0] = cos(rad);
		matrix[0][1] = -sin(rad);
		matrix[1][0] = sin(rad);
		matrix[1][1] = cos(rad);

		return matrix;
    }

    static Matrix4 Rotate(double dx, double dy, double dz) {
		return RotateY(dy)* RotateX(dx) * RotateZ(dz);
    }

    static Matrix4 Scale(double sx, double sy, double sz) {
		auto matrix = GetIdentity();

		matrix[0][0] = sx;
		matrix[1][1] = sy;
		matrix[2][2] = sz;

		return matrix;
    }
};

#endif TRANSFORMATION_H