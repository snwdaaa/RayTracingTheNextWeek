#ifndef VEC4_H
#define VEC4_H

class vec4 {
public:
    double e[4];

    vec4() : e{ 0, 0, 0, 0 } {}

    vec4(double e0, double e1, double e2, double e3) : e{ e0, e1, e2, e3 } {}

    vec4(const vec4& v) {
        e[0] = v.e[0];
        e[1] = v.e[1];
        e[2] = v.e[2];
        e[3] = v.e[3];
    }

    // vec3 또는 point3를 vec4로 변환
    // p == 0이면 vec3
    // p != 1이면 point3
    //vec4(const vec3& v3, const double p) : e{ v3.x(), v3.y(), v3.z(), p } {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }
    double w() const { return e[3]; }

    vec4 operator-() const { return vec4(-e[0], -e[1], -e[2], -e[3]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec4& operator+=(const vec4& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        e[3] += v.e[3];
        return *this;
    }

    vec4& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        e[3] *= t;
        return *this;
    }

    vec4& operator/=(double t) {
        if (std::abs(t) < std::numeric_limits<double>::epsilon())
            throw std::runtime_error("Division by zero in vec4");

        return *this *= 1 / t;
    }

    // 4차원 공간의 길이 계산
    // w 값을 포함하여 계산하므로 벡터 정규화 혹은 길이 비교 로직에서
    // 문제 발생할 수 있음 -> 아래 3차원 공간 길이 계산 함수 사용
    double length() const {
        return std::sqrt(length_squared());
    }

    double length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3] * e[3];
    }

    double length3() const {
        return std::sqrt(length3_squared());
    }

    // 3D 공간에서의 길이를 계산하는 함수
    double length3_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    bool near_zero() const {
        // 벡터가 모든 방향으로 0에 가까우면 true 리턴
        auto s = std::numeric_limits<double>::epsilon();
        return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s) && (std::fabs(e[3]) < s);
    }

    // 랜덤 방향 벡터 생성
    static vec4 random() {
        return vec4(random_double(), random_double(), random_double(), random_double());
    }

    static vec4 random(double min, double max) {
        return vec4(random_double(min, max), random_double(min, max), random_double(min, max), random_double(min, max));
    }
};

// 벡터 유틸리티 함수
inline std::ostream& operator<<(std::ostream& out, const vec4& v) {
    return out << v.e[0] << " " << v.e[1] << " " << v.e[2] << " " << v.e[3];
}

inline vec4 operator+(const vec4& u, const vec4& v) {
    return vec4(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2], u.e[3] + v.e[3]);
}

inline vec4 operator-(const vec4& u, const vec4& v) {
    return vec4(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2], u.e[3] - v.e[3]);
}

inline vec4 operator*(const vec4& u, const vec4& v) {
    return vec4(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2], u.e[3] * v.e[3]);
}

inline vec4 operator*(double t, const vec4& v) {
    return vec4(t * v.e[0], t * v.e[1], t * v.e[2], t * v.e[3]);
}

inline vec4 operator*(const vec4& v, double t) {
    return t * v;
}

inline vec4 operator*(const matrix4& m, const vec4& v) {
    vec4 result;
    for (unsigned int i = 0; i < 4; i++) {
        result.e[i] = m[i][0] * v.e[0] +
            m[i][1] * v.e[1] +
            m[i][2] * v.e[2] +
            m[i][3] * v.e[3];
    }
    return result;
}

inline vec4 operator/(const vec4& v, double t) {
    if (std::abs(t) < std::numeric_limits<double>::epsilon())
        throw std::runtime_error("Division by zero in vec4");

    return (1 / t) * v;
}

inline vec3 to_vec3(const vec4& v) {
    auto x = v.x();
    auto y = v.y();
    auto z = v.z();
    auto w = v.w();

    if (std::abs(w) > std::numeric_limits<double>::epsilon()) // 0이 아니면 (점인 경우)
        return vec3(x / w, y / w, z / w);
    else
        return vec3(x, y, z);
}

inline point3 to_point3(const vec4& v) {
    auto x = v.x();
    auto y = v.y();
    auto z = v.z();
    auto w = v.w();

    if (std::abs(w) > std::numeric_limits<double>::epsilon()) // 0이 아니면 (점인 경우)
        return point3(x / w, y / w, z / w);
    else
        return point3(x, y, z);
}

inline vec4 to_vec4(const vec3& v) {
    return vec4(v.x(), v.y(), v.z(), 0.0);
}

inline vec4 to_vec4(const point3& p) {
    return vec4(p.x(), p.y(), p.z(), 1.0);
}

#endif VEC4_H