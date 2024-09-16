#pragma once

#include <opencv2/opencv.hpp>
#include <bits/stdc++.h>

#define PI					3.14159265358979323
#define DEG2RAD(theta)		(0.01745329251994329 * (theta))
#define FRACTION(v)			((v) - (int)(v))

template<typename T>
concept floating_point = std::is_floating_point_v<T>;

template<typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template< typename T, T N, T Min, T Max>
concept in_range = (N <= Max && N >= Min);

template< typename T, T N, T Min>
concept greater_than = (N >= Min);

template< typename T, T N, T Max>
concept less_than = (N <= Max);

constexpr double eps = 1e-8;

using decimal = double;

template<floating_point T>
bool equal(const T& a, const T&b) {
    return (std::fabs(a - b) < eps);
}

template<floating_point T>
int sign(const T& x) {
    if (x < -eps) return -1;
    if (x > eps) return 1;
    return 0; 
}

template<floating_point T>
int cmp(const T& x, const T& y) {
    if (fabs(x - y) < eps) return 0;
    if (x < y) return -1;
    return 1;
}

template <arithmetic T>
void clamp(T& v, const T& min, const T& max) {
    v = std::min(v, max), v = std::max(v, min);
}


