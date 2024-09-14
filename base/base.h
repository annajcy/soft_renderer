#pragma once

#include <opencv2/opencv.hpp>
#include <bits/stdc++.h>

#define PI					3.14159265358979323
#define DEG2RAD(theta)		(0.01745329251994329 * (theta))
#define FRACTION(v)			((v) - (int)(v))

template<typename T>
concept Floating_point = std::is_floating_point_v<T>;

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

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



