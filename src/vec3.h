#ifndef VEC3_H
#define VEC3_H

class vec3 {
    public:
        double e[3];

        vec3() : e{0, 0, 0} {}
        vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }

        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        vec3& operator+=(const vec3& v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vec3& operator*=(double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator/=(double t) {
            return *this *= 1/t;
        }

        double length() const {
            return std::sqrt(length_squared());
        }

        double length_squared() const {
            return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
        }

        bool near_zero() const {
            // 벡터가 모든 방향으로 0에 가까우면 true 리턴
            auto s = 1e-8;
            return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
        }

        // 랜덤 방향 벡터 생성
        static vec3 random() {
            return vec3(random_double(), random_double(), random_double());
        }

        static vec3 random(double min, double max) {
            return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
        }
};

// point3를 vec3의 alias로 사용
using point3 = vec3;

// 벡터 유틸리티 함수
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << " " << v.e[1] << " " << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

// 1x1 직사각형에서 원 범위 안에 들어오는 랜덤 벡터 생성
inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() < 1)
            return p;
    }
}

// 단위원을 감싸는 큐브 내에서 랜덤한 벡터 생성
// 단위원 안에 있다면 accept -> normalize
// 단위원 밖에 있다면 reject
inline vec3 random_unit_vector() {
    while (true) {
        auto p = vec3::random(-1, 1); // 랜덤 벡터 
        auto lensq = p.length_squared();
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
inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 on_unit_sphere = random_unit_vector(); // 랜덤 벡터 생성
    if (dot(on_unit_sphere, normal) > 0.0) // 법선과 같은 hemisphere에 있음
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

// 벡터 반전
inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

// 벡터 굴절
// 굴절된 벡터 R'을 x와 y 성분으로 나누어 계산 후 합침
// uv와 n 모두 단위 벡터라고 가정
inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    double cos_theta = std::fmin(dot(-uv, n), 1.0); // 최솟값 1
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n); // 수직 성분
    vec3 r_out_parallel = -std::sqrt(
        std::fabs(1.0 - r_out_perp.length_squared())) * n; // 수평 성분
    return r_out_perp + r_out_parallel; // R'
}

#endif