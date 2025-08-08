#ifndef MAT4_H
#define MAT4_H

class matrix4 {
private:
    double m[4][4]; // 4x4 행렬

public:
    // TODO: 기본 생성자 검증하기
    matrix4() {
	// 0으로 초기화
	std::fill(m[0][0], m[3][3], 0);
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

    matrix4(const double* mat_arr) {
	// 4x4 배열을 입력받아 행렬에 저장
	// 이때, 배열은 메모리 상에 연속되어 있어야 함
	// 동적 할당한 배열은 사용하면 안 됨
	memcpy(m, mat_arr, sizeof(double) * 16);
    }
};

#endif