#ifndef POINT3_H
#define POINT3_H

class Point3 {
public:
    double e[3];

    Point3() : e{ 0, 0, 0 } {};
    Point3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    Point3 operator-() const { return Point3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    Point3& operator+=(const Point3& p) {
        e[0] += p.e[0];
        e[1] += p.e[1];
        e[2] += p.e[2];
        return *this;
    }

    Point3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    Point3& operator/=(double t) {
        return *this *= 1 / t;
    }
};

// 유틸리티 함수
inline std::ostream& operator<<(std::ostream& out, const Point3& p) {
    return out << p.e[0] << " " << p.e[1] << " " << p.e[2];
}

inline Point3 operator+(const Point3& p, const Vec3& v) {
    return Point3(p.e[0] + v.e[0], p.e[1] + v.e[1], p.e[2] + v.e[2]);
}

inline Point3 operator+(const Vec3& v, const Point3& p) {
    return p + v;
}

inline Vec3 operator-(const Point3& u, const Point3& v) {
    return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline Point3 operator-(const Point3& p, const Vec3& v) {
    return Point3(p.e[0] - v.e[0], p.e[1] - v.e[1], p.e[2] - v.e[2]);
}

inline Point3 operator*(double t, const Point3& p) {
    return Point3(t * p.e[0], t * p.e[1], t * p.e[2]);
}

inline Point3 operator*(const Point3& p, double t) {
    return t * p;
}

inline Point3 operator/(const Point3& p, double t) {
    if (std::abs(t) < std::numeric_limits<double>::epsilon())
        throw std::runtime_error("Division by zero in Point3");

    return (1 / t) * p;
}

inline double Dot(const Vec3& u, const Point3& v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline Point3 ToPoint3(const Vec3& v) {
    return Point3(v.e[0], v.e[1], v.e[2]);
}

#endif