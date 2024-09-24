#pragma once

#include "base.h"
#include "lerp.h"

namespace math {

	struct BGR {
		u_int8_t b{ 0 }, g{ 0 }, r{ 0 };
		BGR() = default;
	};

	struct Color  {
	public:
		uint8_t r{ 255 }, g{ 255 }, b{ 255 }, a{ 255 };

		Color(): r(255), g(255), b(255), a(255) { }
		explicit Color(decimal alpha): r(255), g(255), b(255), a(255 * alpha) { }
		explicit Color(int a_): r(255), g(255), b(255), a(a_) { }
		Color(int r_, int g_, int b_) : r(r_), g(g_), b(b_), a(255) { }
		Color(int r_, int g_, int b_, int a_) : r(r_), g(g_), b(b_), a(a_) { }
		Color(int r_, int g_, int b_, decimal alpha) : r(r_), g(g_), b(b_), a(255 * alpha) { }

		int R() const { return r; }
		int G() const { return g; }
		int B() const { return b; }
		int A() const { return a; }

		static Color red() { return Color(255, 0, 0); }
		static Color green() { return Color(0, 255, 0); }
		static Color blue() { return Color(0, 0, 255); }

		static Color alpha_blend(const Color& foreground, const Color& background) {
			auto &[r0, g0, b0, _] = background;
			auto &[r1, g1, b1, a1] = foreground;
			decimal alpha1 = a1 / 255.0f;
			uint8_t r = r1 * alpha1 + r0 * (1 - alpha1);
			uint8_t g = g1 * alpha1 + g0 * (1 - alpha1);
			uint8_t b = b1 * alpha1 + b0 * (1 - alpha1);
			return Color(r, g, b);
		}

		static Color interpolate_color(
				const Color& color_a,
				const Color& color_b,
				const Color& color_c,
				const std::tuple<decimal, decimal, decimal>& barycentric
		) {
			auto red = math::calculate_weighed(color_a.R(), color_b.R(), color_c.R(), barycentric);
			auto green = math::calculate_weighed(color_a.G(), color_b.G(), color_c.G(), barycentric);
			auto blue = math::calculate_weighed(color_a.B(), color_b.B(), color_c.B(), barycentric);
			auto alpha = math::calculate_weighed(color_a.A(), color_b.A(), color_c.A(), barycentric);
			return {red, green, blue, alpha};
		}

		static Color interpolate_color(
				const Color& color_a,
				const Color& color_b,
				const std::pair<decimal, decimal>& factor
		) {
			auto red = math::calculate_weighed(color_a.R(), color_b.R(), factor);
			auto green = math::calculate_weighed(color_a.G(), color_b.G(), factor);
			auto blue = math::calculate_weighed(color_a.B(), color_b.B(), factor);
			auto alpha = math::calculate_weighed(color_a.A(), color_b.A(), factor);
			return {red, green, blue, alpha};
		}

		Color operator*(decimal factor) {
			Color result(*this);
			result.a *= factor;
			return result;
		}

		Color operator*=(decimal factor) {
			this->a *= factor;
			return *this;
		}

		friend std::ostream& operator<<(std::ostream& os, const Color& color) {
			std::cout << std::endl;
			std::cout << "RGBA of color:" << std::endl;
			std::cout << color.R() << ' ' << color.G() << ' ' << color.B() << ' ' << color.A() << std::endl;
			return os;
		}

	};
}


