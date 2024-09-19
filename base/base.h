#pragma once

#include <opencv2/opencv.hpp>
#include <bits/stdc++.h>

using decimal = double;

inline decimal PI() { return acos(-1); }
inline decimal deg_to_rad(decimal angle) { return  angle / 180.0 * PI(); }
inline decimal rad_to_deg(decimal rad) { return  rad / PI() * 180.0; }
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



template<Floating_point T>
bool equal(const T& a, const T&b) {
    return (std::fabs(a - b) < eps);
}

template<Floating_point T>
int sign(const T& x) {
    if (x < -eps) return -1;
    if (x > eps) return 1;
    return 0; 
}

template<Floating_point T>
int cmp(const T& x, const T& y) {
    if (fabs(x - y) < eps) return 0;
    if (x < y) return -1;
    return 1;
}

template <Arithmetic T>
void clamp(T& v, const T& min, const T& max) {
    v = std::min(v, max), v = std::max(v, min);
}


