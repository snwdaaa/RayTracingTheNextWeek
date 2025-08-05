#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtw_stb_image.h"

// 텍스처 매핑 핵심 개념
// 3D 표면점 -> 구면 좌표계 -> 텍스처 좌표계 -> 이미지 좌표계
// (x,y,z) -> (theta,phi) -> (u,v) -> (i,j)

class texture {
public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture {
private:
    color albedo;
public:
    solid_color(const color& albedo) : albedo(albedo) {}

    // RGB 값으로 solid 텍스처 객체 생성
    solid_color(double red, double green, double blue)
	: solid_color(color(red, green, blue)) {}

    color value(double u, double v, const point3& p) const override {
	return albedo;
    }
};

class checker_texture : public texture {
private:
    double inv_scale;
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
public:
    checker_texture(double scale, shared_ptr<texture> even, 
	shared_ptr<texture> odd) 
	: inv_scale(1.0 / scale), even(even), odd(odd) { }

    checker_texture(double scale, const color& c1, const color& c2)
	: checker_texture(scale, make_shared<solid_color>(c1), 
	    make_shared<solid_color>(c2)) { }

    color value(double u, double v, const point3& p) const override {
	auto xInteger = int(std::floor(inv_scale * p.x()));
	auto yInteger = int(std::floor(inv_scale * p.y()));
	auto zInteger = int(std::floor(inv_scale * p.z()));
	bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

	return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }
};

class image_texture : public texture {
private:
    rtw_image image;
public:
    image_texture(const char* filename) : image(filename) {}

    color value(double u, double v, const point3& p) const override {
	// 이미지 데이터가 제대로 로드되지 않았다면 cyan 리턴
	if (image.height() <= 0) return color(0, 1, 1);

	// 입력된 u, v 값의 범위를 [0,1]로 고정
	u = interval(0, 1).clamp(u);
	v = interval(0, 1).clamp(v);

	// v 좌표 뒤집기
	// 대부분 이미지 좌표계는 맨 위를 v=0으로 간주
	// 구의 텍스처 좌표계는 맨 아래를 v=0으로 간주
	v = 1.0 - v;

	// uv좌표 -> 픽셀 인덱스 변환
	int i = int(u * image.width());
	int j = int(v * image.height());

	// rtw_image에서 해당 인덱스의 색상 가져온 후
	// [0.0, 1.0] 범위 color 객체로 변환
	auto pixel = image.pixel_data(i, j);
	auto color_scale = 1.0 / 255.0;
	return color(pixel[0] * color_scale, pixel[1] * color_scale, pixel[2] * color_scale);
    }
};

#endif