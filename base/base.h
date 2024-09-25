#pragma once

#include <opencv2/opencv.hpp>
#include <bits/stdc++.h>

using decimal = float;
using varied_type = std::variant<float, double, long double, int, long long, char>;

inline decimal PI() { return acos(-1); }
inline decimal deg_to_rad(decimal angle) { return angle / 180.0 * PI(); }
inline decimal rad_to_deg(decimal rad) { return rad / PI() * 180.0; }
inline decimal fraction(decimal v) { return v - (int)v; }

template<typename T>
concept Floating_point = std::is_floating_point_v<T>;

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<typename T, T N, T Min, T Max>
concept In_range = (N <= Max && N >= Min);

template<typename T, T N, T Min>
concept Greater_than = (N >= Min);

template<typename T, T N, T Max>
concept Less_than = (N <= Max);

template<typename T, T P, T Q>
concept Equal = (P == Q);

constexpr double eps = 1e-8;

inline bool equal(const decimal& a, const decimal& b) {
    return (std::fabs(a - b) < eps);
}

inline int sign(const decimal& x) {
    if (x < -eps) return -1;
    if (x > eps) return 1;
    return 0; 
}

inline int cmp(const decimal& x, const decimal& y) {
    if (fabs(x - y) < eps) return 0;
    if (x < y) return -1;
    return 1;
}

inline void clamp(decimal& v, const decimal& min, const decimal& max) {
    v = std::min(v, max), v = std::max(v, min);
}

inline void clamp(int& v, const int& min, const int& max) {
    v = std::min(v, max), v = std::max(v, min);
}


