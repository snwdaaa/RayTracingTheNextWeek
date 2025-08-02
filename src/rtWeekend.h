#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <algorithm>

// C++ std usings

using std::make_shared;
using std::shared_ptr;

// 상수

const double infinity = std::numeric_limits<double>::infinity(); // double 최댓값
const double pi = 3.1415926535897932385;

// 유틸리티 함수

// 도 -> 라디안 변환
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0f;
}

inline double random_double() {
    // [0,1)에서 랜덤한 실수 리턴
    return std::rand() / (RAND_MAX + 1.0); // 1이 되지 않게 하기 위해 + 1.0
}

inline double random_double(double min, double max) {
    // [min, max)에서 랜덤한 실수 리턴
    return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
    // [min, max]에서 랜덤한 정수 리턴
    return min + (std::rand() % (max - min + 1));
}

// Common Header

#include "color.h"
#include "ray.h"
#include "vec3.h"

#endif