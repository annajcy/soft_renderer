#pragma once

#include "base.h"

struct Color  {
private:
	uint8_t r{ 255 }, g{ 255 }, b{ 255 }, a{ 255 };
public:
    
	Color(): r(255), g(255), b(255), a(255) { }
	explicit Color(float alpha): r(255), g(255), b(255), a(255 * alpha) { }
	Color(int r_, int g_, int b_) : r(r_), g(g_), b(b_), a(255) { }
    Color(int r_, int g_, int b_, int a_) : r(r_), g(g_), b(b_), a(a_) { }
	Color(int r_, int g_, int b_, float alpha) : r(r_), g(g_), b(b_), a(255 * alpha) { }
	
	int R() const { return r; }
	int G() const { return g; }
	int B() const { return b; }
	int A() const { return a; }

	Color operator+(const Color& foreground_color) {
		auto &[r0, g0, b0, a0] = *this;
		auto &[r1, g1, b1, a1] = foreground_color;
		float alpha0 = a0 / 255.0f;
		float alpha1 = a1 / 255.0f;
		uint8_t r = r1 * alpha1 + r0 * (1 - alpha1);
		uint8_t g = g1 * alpha1 + g0 * (1 - alpha1);
		uint8_t b = g1 * alpha1 + b0 * (1 - alpha1);
		uint8_t a = a1 + a0 * (1 - a1);
		return Color(r, g, b, a);
	}

	Color& operator+=(const Color& foreground_color) {
		auto &[r0, g0, b0, a0] = *this;
		auto &[r1, g1, b1, a1] = foreground_color;
		float alpha0 = a0 / 255.0f;
		float alpha1 = a1 / 255.0f;
		r0 = r1 * alpha1 + r0 * (1 - alpha1);
		g0 = g1 * alpha1 + g0 * (1 - alpha1);
		b0 = g1 * alpha1 + b0 * (1 - alpha1);
		a0 = a1 + a0 * (1 - alpha1);
		return *this;
	}

	Color operator*(float factor) {
		Color result(*this);
		result.a *= factor;
		return result;
	}

	friend Color operator*(float factor, Color color) {
		color.a *= factor;
		return color;
	}

	Color& operator*=(float factor) {
		this->a *= factor;
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const Color& color) {
		auto &[r, g, b, a] = color;
		std::cout << std::endl;
		std::cout << "RGBA of color:" << std::endl;
		std::cout << color.R() << ' ' << color.G() << ' ' << color.B() << ' ' << color.A() << std::endl;
		return os;
	}

};
