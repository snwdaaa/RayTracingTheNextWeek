#ifndef RAY_H
#define RAY_H

#include "Vec3.h"

class Ray {
private:
    Point3 orig;
    Vec3 dir;
    double tm; // 레이가 생성된 특정 시각
public:
    Ray() {}

    Ray(const Point3& origin, const Vec3& direction, double time) 
        : orig(origin), dir(direction), tm(time) {}

    Ray(const Point3& origin, const Vec3& direction)
        : orig(origin), dir(direction), tm(0) {}

    const Point3& GetOrigin() const { return orig; }
    const Vec3& GetDirection() const { return dir; }
    double GetTime() const { return tm; }

    Point3 At(double t) const {
        return orig + t*dir;
    }
};

#endif