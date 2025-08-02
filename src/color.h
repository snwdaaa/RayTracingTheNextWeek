#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"

#include <fstream>
#include <vector>
using color = vec3;

// Linear Space -> Gamma Space (Inverse Gamma 2)
inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0) {
	return std::sqrt(linear_component);
    }

    return 0;
}

void write_color(std::vector<color>& value, std::ofstream& out) {
    for (auto& color : value)
    {
	auto r = color.x();
	auto g = color.y();
	auto b = color.z();

	// 선형 공간 값을 Gamma 2로 감마 공간 값으로 바꿈
	r = linear_to_gamma(r);
	g = linear_to_gamma(g);
	b = linear_to_gamma(b);

	// [0,1] 범위 값을 [0,255]로 변환
	static const interval intensity(0.000, 0.999); // 샘플링한 값 평균 저장
	int rbyte = int(256 * intensity.clamp(r));
	int gbyte = int(256 * intensity.clamp(g));
	int bbyte = int(256 * intensity.clamp(b));

	// 픽셀 컬러 컴포넌트 쓰기
	out << rbyte << " " << gbyte << " " << bbyte << "\n";
    }
}

#endif