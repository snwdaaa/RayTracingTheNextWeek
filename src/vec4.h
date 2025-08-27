#ifndef VEC4_H
#define VEC4_H

class Vec4 {
public:
    double e[4];

    Vec4() : e{ 0, 0, 0, 0 } {}

    Vec4(double e0, double e1, double e2, double e3) : e{ e0, e1, e2, e3 } {}

    Vec4(const Vec4& v) {
        e[0] = v.e[0];
        e[1] = v.e[1];
        e[2] = v.e[2];
        e[3] = v.e[3];
    }

    // Vec3 또는 point3를 vec4로 변환
    // p == 0이면 vec3
    // p != 1이면 point3
    //Vec4(const Vec3& v3, const double p) : e{ v3.x(), v3.y(), v3.z(), p } {}

    double GetX() const { return e[0]; }
    double GetY() const { return e[1]; }
    double GetZ() const { return e[2]; }
    double GetW() const { return e[3]; }

    Vec4 operator-() const { return Vec4(-e[0], -e[1], -e[2], -e[3]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    Vec4& operator+=(const Vec4& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        e[3] += v.e[3];
        return *this;
    }

    Vec4& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        e[3] *= t;
        return *this;
    }

    Vec4& operator/=(double t) {
        if (std::abs(t) < std::numeric_limits<double>::epsilon())
            throw std::runtime_error("Division by zero in Vec4");

        return *this *= 1 / t;
    }

    // 4차원 공간의 길이 계산
    // w 값을 포함하여 계산하므로 벡터 정규화 혹은 길이 비교 로직에서
    // 문제 발생할 수 있음 -> 아래 3차원 공간 길이 계산 함수 사용
    double GetLength() const {
        return std::sqrt(GetLengthSquared());
    }

    double GetLengthSquared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3] * e[3];
    }

    double GetLength3() const {
        return std::sqrt(GetLength3Squared());
    }

    // 3D 공간에서의 길이를 계산하는 함수
    double GetLength3Squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    bool IsNearZero() const {
        // 벡터가 모든 방향으로 0에 가까우면 true 리턴
        auto s = std::numeric_limits<double>::epsilon();
        return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s) && (std::fabs(e[3]) < s);
    }

    // 랜덤 방향 벡터 생성
    static Vec4 GetRandomVec4() {
        return Vec4(GetRandomDouble(), GetRandomDouble(), GetRandomDouble(), GetRandomDouble());
    }

    static Vec4 GetRandomVec4(double min, double max) {
        return Vec4(GetRandomDouble(min, max), GetRandomDouble(min, max), GetRandomDouble(min, max), GetRandomDouble(min, max));
    }
};

// 벡터 유틸리티 함수
inline std::ostream& operator<<(std::ostream& out, const Vec4& v) {
    return out << v.e[0] << " " << v.e[1] << " " << v.e[2] << " " << v.e[3];
}

inline Vec4 operator+(const Vec4& u, const Vec4& v) {
    return Vec4(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2], u.e[3] + v.e[3]);
}

inline Vec4 operator-(const Vec4& u, const Vec4& v) {
    return Vec4(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2], u.e[3] - v.e[3]);
}

inline Vec4 operator*(const Vec4& u, const Vec4& v) {
    return Vec4(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2], u.e[3] * v.e[3]);
}

inline Vec4 operator*(double t, const Vec4& v) {
    return Vec4(t * v.e[0], t * v.e[1], t * v.e[2], t * v.e[3]);
}

inline Vec4 operator*(const Vec4& v, double t) {
    return t * v;
}

inline Vec4 operator*(const Matrix4& m, const Vec4& v) {
    Vec4 result;
    for (unsigned int i = 0; i < 4; i++) {
        result.e[i] = m[i][0] * v.e[0] +
            m[i][1] * v.e[1] +
            m[i][2] * v.e[2] +
            m[i][3] * v.e[3];
    }
    return result;
}

inline Vec4 operator/(const Vec4& v, double t) {
    if (std::abs(t) < std::numeric_limits<double>::epsilon())
        throw std::runtime_error("Division by zero in Vec4");

    return (1 / t) * v;
}

inline Vec3 ToVec3(const Vec4& v) {
    auto x = v.GetX();
    auto y = v.GetY();
    auto z = v.GetZ();
    auto w = v.GetW();

    if (std::abs(w) > std::numeric_limits<double>::epsilon()) // 0이 아니면 (점인 경우)
        return Vec3(x / w, y / w, z / w);
    else
        return Vec3(x, y, z);
}

inline Point3 ToPoint3(const Vec4& v) {
    auto x = v.GetX();
    auto y = v.GetY();
    auto z = v.GetZ();
    auto w = v.GetW();

    if (std::abs(w) > std::numeric_limits<double>::epsilon()) // 0이 아니면 (점인 경우)
        return Point3(x / w, y / w, z / w);
    else
        return Point3(x, y, z);
}

inline Vec4 ToVec4(const Vec3& v) {
    return Vec4(v.GetX(), v.GetY(), v.GetZ(), 0.0);
}

inline Vec4 ToVec4(const Point3& p) {
    return Vec4(p.x(), p.y(), p.z(), 1.0);
}

#endif VEC4_H