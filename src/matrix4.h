#ifndef MAT4_H
#define MAT4_H

class matrix4 {
public:
    double m[4][4] = { 0, }; // 4x4 행렬

    matrix4() {}

    matrix4(const matrix4& mat) {
	memcpy(m, mat.m, sizeof(double) * 16);
    }

    matrix4(const std::vector<double>& r1, const std::vector<double>& r2,
	const std::vector<double>& r3, const std::vector<double>& r4)
    {
	// 4개의 행 벡터를 받아 배열의 각 행에 저장
	std::copy(r1.begin(), r1.end(), m[0]);
	std::copy(r2.begin(), r2.end(), m[1]);
	std::copy(r3.begin(), r3.end(), m[2]);
	std::copy(r4.begin(), r4.end(), m[3]);
    }

    const double* operator[](int i) const { return m[i]; }
    double* operator[](int i) { return m[i]; }

    matrix4 operator-() const {
	matrix4 mat4;

	for (unsigned int i = 0; i < 4; i++)
	    for (unsigned int j = 0; j < 4; j++)
		mat4.m[i][j] = -m[i][j];

	return mat4;
    }

    matrix4 operator*(const matrix4& other) const {
	matrix4 mat;

	for (unsigned int i = 0; i < 4; i++)
	    for (unsigned int j = 0; j < 4; j++)
		for (unsigned int k = 0; k < 4; k++)
		    mat.m[i][j] += m[i][k] * other.m[k][j];

	return mat;
    }

    matrix4 operator*=(const matrix4& other) const {
	return (*this) * other;
    }
    
    matrix4 operator+(const matrix4& other) const {
	matrix4 mat;

	for (unsigned int i = 0; i < 4; i++)
	    for (unsigned int j = 0; j < 4; j++)
		mat.m[i][j] = m[i][j] + other[i][j];

	return mat;
    }

    matrix4 operator+=(const matrix4& other) const {
	return (*this) + other;
    }

    matrix4 operator-(const matrix4& other) const {
	matrix4 mat;

	for (unsigned int i = 0; i < 4; i++)
	    for (unsigned int j = 0; j < 4; j++)
		mat.m[i][j] = m[i][j] - other[i][j];

	return mat;
    }

    matrix4 operator-=(const matrix4& other) const {
	return (*this) - other;
    }

    matrix4 operator/(const matrix4& other) const {
	matrix4 mat;

	for (unsigned int i = 0; i < 4; i++)
	    for (unsigned int j = 0; j < 4; j++)
		mat.m[i][j] = m[i][j] / other[i][j];

	return mat;
    }

    matrix4 operator/=(const matrix4& other) const {
	return (*this) / other;
    }

    const double determinant() const {
	// 행렬식 구하기
	double m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
	double m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
	double m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
	double m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];

	return m00 * (m11 * (m22 * m33 - m23 * m32) - m12 * (m21 * m33 - m23 * m31) + m13 * (m21 * m32 - m22 * m31)) -
	    m01 * (m10 * (m22 * m33 - m23 * m32) - m12 * (m20 * m33 - m23 * m30) + m13 * (m20 * m32 - m22 * m30)) +
	    m02 * (m10 * (m21 * m33 - m23 * m31) - m11 * (m20 * m33 - m23 * m30) + m13 * (m20 * m31 - m21 * m30)) -
	    m03 * (m10 * (m21 * m32 - m22 * m31) - m11 * (m20 * m32 - m22 * m30) + m12 * (m20 * m31 - m21 * m30));
    }
};

// 행렬 유틸리티 함수
inline matrix4 operator*(double t, const matrix4& v) {
    matrix4 matrix(v);

    for (unsigned int i = 0; i < 4; i++)
	for (unsigned int j = 0; j < 4; j++)
	    matrix.m[i][j] *= t;

    return matrix;
}

inline matrix4 identity() {
    // Identity Matrix 리턴

    std::vector<std::vector<double>> identity_mat_vec = {
	{1.0, 0.0, 0.0, 0.0},
	{0.0, 1.0, 0.0, 0.0},
	{0.0, 0.0, 1.0, 0.0},
	{0.0, 0.0, 0.0, 1.0}
    };

    matrix4 matrix(
	identity_mat_vec[0],
	identity_mat_vec[1],
	identity_mat_vec[2],
	identity_mat_vec[3]
    );

    return matrix;
}

inline matrix4 adjoint(const matrix4& mat) {
    double m00 = mat.m[0][0], m01 = mat.m[0][1], m02 = mat.m[0][2], m03 = mat.m[0][3];
    double m10 = mat.m[1][0], m11 = mat.m[1][1], m12 = mat.m[1][2], m13 = mat.m[1][3];
    double m20 = mat.m[2][0], m21 = mat.m[2][1], m22 = mat.m[2][2], m23 = mat.m[2][3];
    double m30 = mat.m[3][0], m31 = mat.m[3][1], m32 = mat.m[3][2], m33 = mat.m[3][3];

    matrix4 adj;
    adj[0][0] = m11 * (m22 * m33 - m23 * m32) - m12 * (m21 * m33 - m23 * m31) + m13 * (m21 * m32 - m22 * m31);
    adj[0][1] = -m01 * (m22 * m33 - m23 * m32) + m02 * (m21 * m33 - m23 * m31) - m03 * (m21 * m32 - m22 * m31);
    adj[0][2] = m01 * (m12 * m33 - m13 * m32) - m02 * (m11 * m33 - m13 * m31) + m03 * (m11 * m32 - m12 * m31);
    adj[0][3] = -m01 * (m12 * m23 - m13 * m22) + m02 * (m11 * m23 - m13 * m21) - m03 * (m11 * m22 - m12 * m21);

    adj[1][0] = -m10 * (m22 * m33 - m23 * m32) + m12 * (m20 * m33 - m23 * m30) - m13 * (m20 * m32 - m22 * m30);
    adj[1][1] = m00 * (m22 * m33 - m23 * m32) - m02 * (m20 * m33 - m23 * m30) + m03 * (m20 * m32 - m22 * m30);
    adj[1][2] = -m00 * (m12 * m33 - m13 * m32) + m02 * (m10 * m33 - m13 * m30) - m03 * (m10 * m32 - m12 * m30);
    adj[1][3] = m00 * (m12 * m23 - m13 * m22) - m02 * (m10 * m23 - m13 * m20) + m03 * (m10 * m22 - m12 * m20);

    adj[2][0] = m10 * (m21 * m33 - m23 * m31) - m11 * (m20 * m33 - m23 * m30) + m13 * (m20 * m31 - m21 * m30);
    adj[2][1] = -m00 * (m21 * m33 - m23 * m31) + m01 * (m20 * m33 - m23 * m30) - m03 * (m20 * m31 - m21 * m30);
    adj[2][2] = m00 * (m11 * m33 - m13 * m31) - m01 * (m10 * m33 - m13 * m30) + m03 * (m10 * m31 - m11 * m30);
    adj[2][3] = -m00 * (m11 * m23 - m13 * m21) + m01 * (m10 * m23 - m13 * m20) - m03 * (m10 * m21 - m11 * m20);

    adj[3][0] = -m10 * (m21 * m32 - m22 * m31) + m11 * (m20 * m32 - m22 * m30) - m12 * (m20 * m31 - m21 * m30);
    adj[3][1] = m00 * (m21 * m32 - m22 * m31) - m01 * (m20 * m32 - m22 * m30) + m02 * (m20 * m31 - m21 * m30);
    adj[3][2] = -m00 * (m11 * m32 - m12 * m31) + m01 * (m10 * m32 - m12 * m30) - m02 * (m10 * m31 - m11 * m30);
    adj[3][3] = m00 * (m11 * m22 - m12 * m21) - m01 * (m10 * m22 - m12 * m20) + m02 * (m10 * m21 - m11 * m20);

    return adj;
}

inline matrix4 inverse(const matrix4& mat) {
    double det = mat.determinant();
    if (std::abs(det) < 1e-8) { // 행렬식이 0이면 분자가 0이 됨
	return identity(); // 임시로 항등행렬 리턴
    }

    matrix4 adj = adjoint(mat);
    return (1 / det) * adj;
}

inline matrix4 transpose(const matrix4& mat) {
    matrix4 t(mat);

    for (unsigned int i = 0; i < 4; i++)
	for (unsigned int j = 0; j < 4; j++)
	    t.m[i][j] = t.m[j][i];

    return t;
}

#endif