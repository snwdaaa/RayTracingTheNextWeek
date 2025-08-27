#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

class ConstantMedium : public Hittable {
private:
    shared_ptr<Hittable> boundary;
    double negInvDensity;
    shared_ptr<Material> phaseFunction;
public:
    ConstantMedium(shared_ptr<Hittable> boundary, 
	double density, shared_ptr<Texture> tex)
	: boundary(boundary), negInvDensity(-1 / density),
	phaseFunction(make_shared<Isotropic>(tex))
    {}

    ConstantMedium(shared_ptr<Hittable> boundary,
	double density, const Color& albedo)
	: boundary(boundary), negInvDensity(-1 / density),
	phaseFunction(make_shared<Isotropic>(albedo))
    {}

    bool Hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
	    HitRecord rec1, rec2;

	    // boundary에서 ray의 진입/탈출 지점을 구함
        if (!boundary->Hit(r, Interval::universe, rec1))
            return false;

        if (!boundary->Hit(r, Interval(rec1.t + 0.0001, infinity), rec2))
            return false;

	    // 범위 설정
	    if (rec1.t < ray_t.min) rec1.t = ray_t.min;
	    if (rec2.t > ray_t.max) rec2.t = ray_t.max;

	    // 진입 지점이 탈출 지점보다 앞에 있으면 false
        if (rec1.t >= rec2.t)
            return false;

        if (rec1.t < 0)
            rec1.t = 0;
        auto rayLength = r.GetDirection().GetLength();
        auto distanceInsideBoundary = (rec2.t - rec1.t) * rayLength;
        auto hitDistance = negInvDensity * std::log(GetRandomDouble());

        if (hitDistance > distanceInsideBoundary)
            return false;

        rec.t = rec1.t + hitDistance / rayLength;
        rec.p = r.At(rec.t);

        rec.normal = Vec3(1, 0, 0);
        rec.frontFace = true;
        rec.mat = phaseFunction;

        return true;
    }

    AABB BoundingBox() const override { return boundary->BoundingBox(); }
};

#endif