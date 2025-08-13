#ifndef POINT3_H
#define POINT3_H

#include "vec3.h"

class point3 {
public:
    double e[3];

    point3() : e{ 0, 0, 0 } {};
    point3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    point3 operator-() const { return point3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    point3& operator+=(const point3& p) {
        e[0] += p.e[0];
        e[1] += p.e[1];
        e[2] += p.e[2];
        return *this;
    }

    point3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    point3& operator/=(double t) {
        return *this *= 1 / t;
    }
};

// 유틸리티 함수
inline std::ostream& operator<<(std::ostream& out, const point3& p) {
    return out << p.e[0] << " " << p.e[1] << " " << p.e[2];
}

inline point3 operator+(const point3& p, const vec3& v) {
    return point3(p.e[0] + v.e[0], p.e[1] + v.e[1], p.e[2] + v.e[2]);
}

inline point3 operator+(const vec3& v, const point3& p) {
    return p + v;
}

inline vec3 operator-(const point3& u, const point3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline point3 operator-(const point3& p, const vec3& v) {
    return point3(p.e[0] - v.e[0], p.e[1] - v.e[1], p.e[2] - v.e[2]);
}

inline point3 operator*(double t, const point3& p) {
    return point3(t * p.e[0], t * p.e[1], t * p.e[2]);
}

inline point3 operator*(const point3& p, double t) {
    return t * p;
}

inline point3 operator/(const point3& p, double t) {
    if (std::abs(t) < std::numeric_limits<double>::epsilon())
        throw std::runtime_error("Division by zero in point3");

    return (1 / t) * p;
}

inline double dot(const vec3& u, const point3& v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline point3 to_point3(const vec3& v) {
    return point3(v.e[0], v.e[1], v.e[2]);
}

#endif