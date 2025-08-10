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
    
    matrix4 operator+(const matrix4& other) const {
	matrix4 mat;

	for (unsigned int i = 0; i < 4; i++)
	    for (unsigned int j = 0; j < 4; j++)
		mat.m[i][j] = m[i][j] + other[i][j];

	return mat;
    }

    matrix4 operator-(const matrix4& other) const {
	matrix4 mat;

	for (unsigned int i = 0; i < 4; i++)
	    for (unsigned int j = 0; j < 4; j++)
		mat.m[i][j] = m[i][j] - other[i][j];

	return mat;
    }

    matrix4 operator/(const matrix4& other) const {
	matrix4 mat;

	for (unsigned int i = 0; i < 4; i++)
	    for (unsigned int j = 0; j < 4; j++)
		mat.m[i][j] = m[i][j] / other[i][j];

	return mat;
    }
};
#endif