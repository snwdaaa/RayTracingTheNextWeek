#ifndef VEC3_H
#define VEC3_H

class Vec3 {
public:
    double e[3];

    Vec3() : e{0, 0, 0} {}
    Vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double GetX() const { return e[0]; }
    double GetY() const { return e[1]; }
    double GetZ() const { return e[2]; }

    Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    Vec3& operator+=(const Vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    Vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    Vec3& operator/=(double t) {
        if (std::abs(t) < std::numeric_limits<double>::epsilon())
            throw std::runtime_error("Division by zero in Vec3");

        return *this *= 1/t;
    }

    double GetLength() const {
        return std::sqrt(GetLengthSquared());
    }

    double GetLengthSquared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    bool IsNearZero() const {
        // 벡터가 모든 방향으로 0에 가까우면 true 리턴
        auto s = std::numeric_limits<double>::epsilon();
        return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
    }

    // 랜덤 방향 벡터 생성
    static Vec3 GetRandomVec3() {
        return Vec3(GetRandomDouble(), GetRandomDouble(), GetRandomDouble());
    }

    static Vec3 GetRandomVec3(double min, double max) {
        return Vec3(GetRandomDouble(min, max), GetRandomDouble(min, max), GetRandomDouble(min, max));
    }
};

// 벡터 유틸리티 함수
inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
    return out << v.e[0] << " " << v.e[1] << " " << v.e[2];
}

inline Vec3 operator+(const Vec3& u, const Vec3& v) {
    return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline Vec3 operator-(const Vec3& u, const Vec3& v) {
    return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline Vec3 operator*(const Vec3& u, const Vec3& v) {
    return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline Vec3 operator*(double t, const Vec3& v) {
    return Vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline Vec3 operator*(const Vec3& v, double t) {
    return t * v;
}

inline Vec3 operator/(const Vec3& v, double t) {
    if (std::abs(t) < std::numeric_limits<double>::epsilon())
        throw std::runtime_error("Division by zero in Vec3");

    return (1/t) * v;
}

inline double Dot(const Vec3& u, const Vec3& v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline Vec3 Cross(const Vec3& u, const Vec3& v) {
    return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline Vec3 GetUnitVector(const Vec3& v) {
    double len = v.GetLength();
    if (len < std::numeric_limits<double>::epsilon())
        return Vec3(0, 0, 0);

    return v / len;
}

// 1x1 직사각형에서 원 범위 안에 들어오는 랜덤 벡터 생성
inline Vec3 RandomInUnitDisk() {
    while (true) {
        auto p = Vec3(GetRandomDouble(-1, 1), GetRandomDouble(-1, 1), 0);
        if (p.GetLengthSquared() < 1)
            return p;
    }
}

// 단위원을 감싸는 큐브 내에서 랜덤한 벡터 생성
// 단위원 안에 있다면 accept -> normalize
// 단위원 밖에 있다면 reject
inline Vec3 GetRandomUnitVector() {
    while (true) {
        auto p = Vec3::GetRandomVec3(-1, 1); // 랜덤 벡터 
        auto lensq = p.GetLengthSquared();
        // 매우 작은 float는 제곱하면 0이 될 수 있음
        // 1e-160보다 작은 수는 무시해서 sqrt(0)이 실행되지 않게 함
        if (1e-160 < lensq && lensq <= 1) { // 벡터가 단위원 안에 있으면
            return p / sqrt(lensq); // 정규화 (단위 벡터로 만듦)
        }
    }
}

// 법선 벡터와의 내적을 통해 올바른 hemisphere에 있는지 확인
// 내적값 > 0 -> OK
// 내적값 < 0 -> invert
inline Vec3 GetRandomVec3OnHemisphere(const Vec3& normal) {
    Vec3 onUnitSphere = GetRandomUnitVector(); // 랜덤 벡터 생성
    if (Dot(onUnitSphere, normal) > 0.0) // 법선과 같은 hemisphere에 있음
        return onUnitSphere;
    else
        return -onUnitSphere;
}

// 벡터 반전
inline Vec3 Reflect(const Vec3& v, const Vec3& n) {
    return v - 2 * Dot(v, n) * n;
}

// 벡터 굴절
// 굴절된 벡터 R'을 x와 y 성분으로 나누어 계산 후 합침
// uv와 n 모두 단위 벡터라고 가정
inline Vec3 Refract(const Vec3& uv, const Vec3& n, double etai_over_etat) {
    double cosTheta = std::fmin(Dot(-uv, n), 1.0); // 최솟값 1
    Vec3 rOutPerpendicular = etai_over_etat * (uv + cosTheta * n); // 수직 성분
    Vec3 rOutParallel = -std::sqrt(
        std::fabs(1.0 - rOutPerpendicular.GetLengthSquared())) * n; // 수평 성분
    return rOutPerpendicular + rOutParallel; // R'
}

#endif