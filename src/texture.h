#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtw_stb_image.h"
#include "Common.h"

// 텍스처 매핑 핵심 개념
// 3D 표면점 -> 구면 좌표계 -> 텍스처 좌표계 -> 이미지 좌표계
// (x,y,z) -> (theta,phi) -> (u,v) -> (i,j)

class Texture {
public:
    virtual ~Texture() = default;

    virtual Color GetColorValue(double u, double v, const Point3& p) const = 0;
};

class SolidColor : public Texture {
private:
	Color albedo;
public:
    SolidColor(const Color& albedo) : albedo(albedo) {}

    // RGB 값으로 solid 텍스처 객체 생성
    SolidColor(double red, double green, double blue)
	: SolidColor(Color(red, green, blue)) {}

	Color GetColorValue(double u, double v, const Point3& p) const override {
		return albedo;
    }
};

class CheckerTexture : public Texture {
private:
    double invScale;
    shared_ptr<Texture> even;
    shared_ptr<Texture> odd;
public:
    CheckerTexture(double scale, shared_ptr<Texture> even, 
	shared_ptr<Texture> odd) 
	: invScale(1.0 / scale), even(even), odd(odd) { }

    CheckerTexture(double scale, const Color& c1, const Color& c2)
	: CheckerTexture(scale, make_shared<SolidColor>(c1), 
	    make_shared<SolidColor>(c2)) { }

	Color GetColorValue(double u, double v, const Point3& p) const override {
		auto xInteger = int(std::floor(invScale * p.x()));
		auto yInteger = int(std::floor(invScale * p.y()));
		auto zInteger = int(std::floor(invScale * p.z()));
		bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

		return isEven ? even->GetColorValue(u, v, p) : odd->GetColorValue(u, v, p);
    }
};

class ImageTexture : public Texture {
private:
    rtw_image image;
public:
    ImageTexture(const char* filename) : image(filename) {}

	Color GetColorValue(double u, double v, const Point3& p) const override {
		// 이미지 데이터가 제대로 로드되지 않았다면 cyan 리턴
		if (image.height() <= 0) return Color(0, 1, 1);

		// 입력된 u, v 값의 범위를 [0,1]로 고정
		u = Interval(0, 1).Clamp(u);
		v = Interval(0, 1).Clamp(v);

		// v 좌표 뒤집기
		// 대부분 이미지 좌표계는 맨 위를 v=0으로 간주
		// 구의 텍스처 좌표계는 맨 아래를 v=0으로 간주
		v = 1.0 - v;

		// uv좌표 -> 픽셀 인덱스 변환
		int i = int(u * image.width());
		int j = int(v * image.height());

		// rtw_image에서 해당 인덱스의 색상 가져온 후
		// [0.0, 1.0] 범위 Color 객체로 변환
		auto pixel = image.pixel_data(i, j);
		auto colorScale = 1.0 / 255.0;
		return Color(pixel[0] * colorScale, pixel[1] * colorScale, pixel[2] * colorScale);
    }
};

#endif