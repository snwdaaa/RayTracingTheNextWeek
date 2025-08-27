#ifndef MATERIAL_H
#define MATERIAL_H

#include "Hittable.h"
#include "Texture.h"

// 레이와 부딪혔을 때 모든 머티리얼의 역할
// 1. 산란광(scattered light) 만들기
// 2. 산란됐을 때, 레이가 얼마나 감소하는지 결정

class Material {
public:
    virtual ~Material() = default;

    // 산란광 만들기
    // 앞으로 만들 모든 머티리얼(lambertian, metal 등)이
    // Scatter 메서드를 각자의 방식대로 구현
    virtual bool Scatter(
	const Ray& rayIn, const HitRecord& rec, Color& attenuation,
	Ray& scattered) const 
	{
		return false;
    }

    // 물체가 방출하는 빛
    virtual Color Emit(double u, double v, const Point3& p) const {
		return Color(0, 0, 0);
    }
};

// Lambertian(diffuse) Reflectance
class Lambertian : public Material{
private:
    //Color albedo; // 물체 고유의 색 or 반사율
    shared_ptr<Texture> tex;
public:
    Lambertian(const Color& albedo) : tex(make_shared<SolidColor>(albedo)) {}
    Lambertian(shared_ptr<Texture> tex) : tex(tex) {}

    // Diffuse Scatter
    bool Scatter(const Ray& rayIn, const HitRecord& rec, Color& attenuation,
	Ray& scattered) const override 
	{
		// Simple Diffuse
		// 충돌 지점의 법선 벡터가 속한 반구에서 랜덤 방향의 벡터 가져옴
		//Vec3 direction = GetRandomVec3OnHemisphere(rec.normal);

		// True Lambertian Reflection
		// 법선 벡터 주변으로 랜덤한 단위벡터 더함
		Vec3 scatteredDir = rec.normal + GetRandomUnitVector();

		// 랜덤 벡터와 노멀 벡터가 정확히 반대 방향인 경우
		// 합이 0이 되어 나중에 오류 유발 할 수 있음
		// 이를 해결하기 위해 scatter된 방향이 0에 가까우면
		// 그냥 노멀 벡터 사용
		if (scatteredDir.IsNearZero()) {
			scatteredDir = rec.normal;
		}

		scattered = Ray(rec.p, scatteredDir, rayIn.GetTime());
		attenuation = tex->GetColorValue(rec.u, rec.v, rec.p);
		return true;
    }
};

class Metal : public Material {
private:
    Color albedo;
    double fuzz; // 0이면 완벽한 거울, 1이면 난반사
public:
    Metal(const Color& albedo, double fuzz) 
	: albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool Scatter(const Ray& rayIn, const HitRecord& rec, Color& attenuation,
	Ray& scattered) const override 
	{
		Vec3 reflected = Reflect(rayIn.GetDirection(), rec.normal);
		// 완벽한 반사 방향 벡터에 fuzz만큼의 무작위 벡터 더함
		reflected = GetUnitVector(reflected) + (fuzz * GetRandomUnitVector());
		scattered = Ray(rec.p, reflected, rayIn.GetTime());
		attenuation = albedo;
		return (Dot(scattered.GetDirection(), rec.normal) > 0);
    }
};

class Dielectric : public Material {
private:
    double refractionIndex; // 진공에서의 굴절률

    static double Reflectance(double cosine, double refractionIndex) {
		// 반사 효과 -> 슐릭 근사 사용
		auto r0 = (1 - refractionIndex) / (1 + refractionIndex);
		r0 = r0 * r0;
		return r0 + (1 - r0) * std::pow(1 - cosine, 5);
    }
public:
    Dielectric(double refractionIndex) : refractionIndex(refractionIndex) {}

    bool Scatter(const Ray& rayIn, const HitRecord& rec, Color& attenuation,
	Ray& scattered) const override 
	{
		attenuation = Color(1.0, 1.0, 1.0);
		// 레이가 물체 안으로 들어가는지, 밖으로 나가는지에 따라
		// 굴절률 다르게 설정
		double ri = rec.frontFace ? (1.0 / refractionIndex) : refractionIndex;
		Vec3 unitDirection = GetUnitVector(rayIn.GetDirection());

		// 레이가 굴절할 수 있는지 결정
		// 단위 벡터인 경우 u dot v = cos(theta) (theta: u와 v 사이 각도)
		double cosTheta = std::fmin(Dot(-unitDirection, rec.normal), 1.0);
		double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
		Vec3 direction;

		// 해가 없는 경우 or 계산된 반사율에 따라 확률적으로 반사 또는 굴절
		if (ri * sinTheta > 1.0 || 
			Reflectance(cosTheta, ri) > GetRandomDouble()) {
			direction = Reflect(unitDirection, rec.normal); // 전반사
		}
		else {
			direction = Refract(unitDirection, rec.normal, ri); // 굴절
		}

		scattered = Ray(rec.p, direction, rayIn.GetTime()); // 굴절된 방향으로 레이 발사

		return true;
    }
};

class DiffuseLight : public Material {
private:
    shared_ptr<Texture> tex;
public:
    DiffuseLight(shared_ptr<Texture> tex) : tex(tex) {}
    DiffuseLight(const Color& emit) : tex(make_shared<SolidColor>(emit)) {}

    Color Emit(double u, double v, const Point3& p) const override {
		return tex->GetColorValue(u, v, p);
    }
};

class Isotropic : public Material {
private:
    shared_ptr<Texture> tex;
public:
    Isotropic(const Color& albedo) : tex(make_shared<SolidColor>(albedo)) {}
    Isotropic(shared_ptr<Texture> tex) : tex(tex) {}

    bool Scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
	Ray& scattered) const override 
	{
		scattered = Ray(rec.p, GetRandomUnitVector(), r_in.GetTime());
		attenuation = tex->GetColorValue(rec.u, rec.v, rec.p);
		return true;
    }
};

#endif