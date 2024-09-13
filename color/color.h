#pragma once

#include "base.h"

struct Color  {
public:
    uint8_t r{ 255 }, g{ 255 }, b{ 255 }, a{ 255 };
	Color(): r(255), g(255), b(255), a(255) { }
	explicit Color(float alpha): r(255), g(255), b(255), a(255 * alpha) { }
	Color(int r_, int g_, int b_) : r(r_), g(g_), b(b_), a(255) { }
    Color(int r_, int g_, int b_, int a_) : r(r_), g(g_), b(b_), a(a_) { }
	Color(int r_, int g_, int b_, float alpha) : r(r_), g(g_), b(b_), a(255 * alpha) { }
};
