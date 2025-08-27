#ifndef COLOR_H
#define COLOR_H

#include "Interval.h"
#include "Vec3.h"

#include <fstream>
#include <vector>
using Color = Vec3;

// Linear Space -> Gamma Space (Inverse Gamma 2)
inline double LinearToGamma(double linearComponent) {
    if (linearComponent > 0) {
		return std::sqrt(linearComponent);
    }

    return 0;
}

void WriteColor(std::vector<Color>& value, std::ofstream& out) {
    for (auto& Color : value)
    {
		auto r = Color.GetX();
		auto g = Color.GetY();
		auto b = Color.GetZ();

		// 선형 공간 값을 Gamma 2로 감마 공간 값으로 바꿈
		r = LinearToGamma(r);
		g = LinearToGamma(g);
		b = LinearToGamma(b);

		// [0,1] 범위 값을 [0,255]로 변환
		static const Interval intensity(0.000, 0.999); // 샘플링한 값 평균 저장
		int rByte = int(256 * intensity.Clamp(r));
		int gByte = int(256 * intensity.Clamp(g));
		int bByte = int(256 * intensity.Clamp(b));

		// 픽셀 컬러 컴포넌트 쓰기
		out << rByte << " " << gByte << " " << bByte << "\n";
    }
}

#endif