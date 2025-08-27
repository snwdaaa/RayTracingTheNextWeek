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
#include <stdexcept>

// C++ std usings

using std::make_shared;
using std::shared_ptr;

// 상수

const double infinity = std::numeric_limits<double>::infinity(); // double 최댓값
const double pi = 3.1415926535897932385;

// 유틸리티 함수

// 도 -> 라디안 변환
inline double DegreesToRadians(double degrees) {
    return degrees * pi / 180.0f;
}

inline double GetRandomDouble() {
    // [0,1)에서 랜덤한 실수 리턴
    return std::rand() / (RAND_MAX + 1.0); // 1이 되지 않게 하기 위해 + 1.0
}

inline double GetRandomDouble(double min, double max) {
    // [min, max)에서 랜덤한 실수 리턴
    return min + (max - min) * GetRandomDouble();
}

inline int GetRandomInt(int min, int max) {
    // [min, max]에서 랜덤한 정수 리턴
    return min + (std::rand() % (max - min + 1));
}

// Common Header

#include "Vec3.h"
#include "Point3.h"
#include "Matrix4.h"
#include "Vec4.h"
#include "Color.h"
#include "Ray.h"

#endif