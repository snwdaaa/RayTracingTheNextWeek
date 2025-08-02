#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

// 레이와 부딪혔을 때 모든 머티리얼의 역할
// 1. 산란광(scattered light) 만들기
// 2. 산란됐을 때, 레이가 얼마나 감소하는지 결정

class material {
public:
    virtual ~material() = default;

    // 산란광 만들기
    // 앞으로 만들 모든 머티리얼(lambertian, metal 등)이
    // scatter 메서드를 각자의 방식대로 구현
    virtual bool scatter(
	const ray& r_in, const hit_record& rec, color& attenuation,
	ray& scattered
    ) const {
	return false;
    }
};

// Lambertian(diffuse) reflectance
class lambertian : public material{
private:
    color albedo; // 물체 고유의 색 or 반사율
public:
    lambertian(const color& albedo) : albedo(albedo) {}

    // Diffuse Scatter
    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation,
	ray& scattered
    ) const override {
	// Simple Diffuse
	// 충돌 지점의 법선 벡터가 속한 반구에서 랜덤 방향의 벡터 가져옴
	//vec3 direction = random_on_hemisphere(rec.normal);

	// True Lambertian Reflection
	// 법선 벡터 주변으로 랜덤한 단위벡터 더함
	vec3 scattered_dir = rec.normal + random_unit_vector();

	// 랜덤 벡터와 노멀 벡터가 정확히 반대 방향인 경우
	// 합이 0이 되어 나중에 오류 유발 할 수 있음
	// 이를 해결하기 위해 scatter된 방향이 0에 가까우면
	// 그냥 노멀 벡터 사용
	if (scattered_dir.near_zero()) {
	    scattered_dir = rec.normal;
	}

	scattered = ray(rec.p, scattered_dir);
	attenuation = albedo;
	return true;
    }
};

class metal : public material {
private:
    color albedo;
    double fuzz; // 0이면 완벽한 거울, 1이면 난반사
public:
    metal(const color& albedo, double fuzz) 
	: albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation,
	ray& scattered
    ) const override {
	vec3 reflected = reflect(r_in.direction(), rec.normal);
	// 완벽한 반사 방향 벡터에 fuzz만큼의 무작위 벡터 더함
	reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
	scattered = ray(rec.p, reflected);
	attenuation = albedo;
	return (dot(scattered.direction(), rec.normal) > 0);
    }
};

class dielectric : public material {
private:
    double refraction_index; // 진공에서의 굴절률

    static double reflectance(double cosine, double refraction_index) {
	// 반사 효과 -> 슐릭 근사 사용
	auto r0 = (1 - refraction_index) / (1 + refraction_index);
	r0 = r0 * r0;
	return r0 + (1 - r0) * std::pow(1 - cosine, 5);
    }
public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation,
	ray& scattered
    ) const override {
	attenuation = color(1.0, 1.0, 1.0);
	// 레이가 물체 안으로 들어가는지, 밖으로 나가는지에 따라
	// 굴절률 다르게 설정
	double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;
	vec3 unit_direction = unit_vector(r_in.direction());

	// 레이가 굴절할 수 있는지 결정
	// 단위 벡터인 경우 u dot v = cos(theta) (theta: u와 v 사이 각도)
	double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
	double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
	vec3 direction;

	// 해가 없는 경우 or 계산된 반사율에 따라 확률적으로 반사 또는 굴절
	if (ri * sin_theta > 1.0 || 
	    reflectance(cos_theta, ri) > random_double()) {
	    direction = reflect(unit_direction, rec.normal); // 전반사
	}
	else {
	    direction = refract(unit_direction, rec.normal, ri); // 굴절
	}

	scattered = ray(rec.p, direction); // 굴절된 방향으로 레이 발사

	// (임시) 무조건 굴절 하게 하기
	return true;
    }
};

#endif