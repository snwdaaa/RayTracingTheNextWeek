#ifndef VEC4_H
#define VEC4_H

class vec4 {
public:
    double e[4];

    vec4() : e{ 0, 0, 0, 0 } {}

    vec4(double e0, double e1, double e2, double e3) : e{ e0, e1, e2, e3 } {}

    // vec3을 vec4로 변환
    vec4(const vec3& v3) : e{ v3.x(), v3.y(), v3.z(), 0.0 } {}

    // point3을 vec4로 변환
    vec4(const point3& p3) : e{ p3.x(), p3.y(), p3.z(), 1.0 } {}

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
        return *this *= 1 / t;
    }

    double length() const {
        return std::sqrt(length_squared());
    }

    double length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2] + e[3] * e[3];
    }

    bool near_zero() const {
        // 벡터가 모든 방향으로 0에 가까우면 true 리턴
        auto s = 1e-8;
        return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s) && (std::fabs(e[3]) < s);
    }

    vec3 to_vec3() const {
        auto w = e[3];

        if (std::abs(w) > 1e-8) // 0이 아니면 (점인 경우)
            return vec3(e[0] / w, e[1] / w, e[2] / w);
        else
            return vec3(e[0], e[1], e[2]);
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

inline vec4 operator/(const vec4& v, double t) {
    return (1 / t) * v;
}

#endif VEC4_H