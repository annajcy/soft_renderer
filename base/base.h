#pragma once

#define PI					3.14159265358979323
#define DEG2RAD(theta)		(0.01745329251994329 * (theta))
#define FRACTION(v)			((v) - (int)(v))

struct Color  {
public:
    uint8_t r{ 255 }, g{ 255 }, b{ 255 }, a{ 255 };
    Color() = default;
	Color(int r_, int g_, int b_) : r(r_), g(g_), b(b_), a(255) { }
    Color(int r_, int g_, int b_, int a_) : r(r_), g(g_), b(b_), a(a_) { }
};
