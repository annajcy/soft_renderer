#pragma once

#include <utility>

#include "base.h"
#include "vec.h"
#include "mat.h"

namespace math {

	using Pixel = Vec2i;
	using Point2d = Vec2f;
	using Point3d = Vec3f;
	using Vector2d = Vec2f;
	using Vector3d = Vec3f;
	using Color_decimal = Vec4f;
	using UV = Vec2f;

	template<typename T>
	inline T cross(const Vec<T, 2>& a, const Vec<T, 2>& b) {
		return a.x() * b.y() - a.y() * b.x();
	} 

	template<typename T>
	inline Vec<T, 3> cross(const Vec<T, 3>& a, const Vec<T, 3>& b) {
		return Vec<T, 3>{a.y() * b.z() - a.z() * b.y(), -(a.x() * b.z() - a.z() * b.x()), a.x() * b.y() - a.y() * b.x()};
	}

	inline void sample_pixel(std::vector<Point2d>& result, const Pixel& pixel, int scale) {
		result.clear();
		decimal stride = 1.0 / scale;
		decimal startup = stride / 2;

		decimal x = startup + pixel.x();
		for (int i = 0; i < scale; i ++, x += stride) {
			decimal y = startup + pixel.y();
			for (int j = 0; j < scale; j ++, y += stride) {
				result.push_back({x, y});
			}
		}
	}

	inline Point2d pixel_to_point2d(const Pixel& pixel) {
		return Point2d({pixel.x() + 0.5, pixel.y() + 0.5});
	}

	inline Pixel point2d_to_pixel(const Point2d& point) {
		auto left = std::floor(point.x()), right = std::floor(point.x() + 1);
		auto bottom = std::floor(point.y()), top = std::floor(point.y() + 1);
		auto to_left = point.x() - left;
		auto to_right = right - point.x();
		auto to_bottom = point.y() - bottom;
		auto to_top = top - point.y();
		int x, y;
		if (to_left > to_right) x = right;
		else x = left;
		if (to_bottom > to_top) y = top;
		else y = bottom;
		return Pixel({x, y});
	}

	struct Triangle2d {
		Point2d a, b, c;

		Triangle2d() = default;
		Triangle2d(Point2d a_, Point2d b_, Point2d c_) : a(std::move(a_)), b(std::move(b_)), c(std::move(c_)) {}
		Triangle2d(const Pixel& a_, const Pixel& b_, const Pixel& c_) : a(pixel_to_point2d(a_)), b(pixel_to_point2d(b_)), c(pixel_to_point2d(c_)) {}

		[[nodiscard]] decimal area() const {
			Vector2d ab = b - a, ac = c - a;
			return std::fabs(cross(ab, ac)) / 2;
		}

		[[nodiscard]] bool enclose(const Point2d& p) const {
			Vector2d pa = a - p, pb = b - p, pc = c - p;
			bool all_positive = cross(pa, pb) > 0 && cross(pb, pc) > 0 && cross(pc, pa) > 0;
			bool all_negative = cross(pa, pb) < 0 && cross(pb, pc) < 0 && cross(pc, pa) < 0;
			return all_positive || all_negative;
		}

		[[nodiscard]] std::pair<Pixel, Pixel> bounding_box() const {
			int min_x = std::min({a.x(), b.x(), c.x()});
			int min_y = std::min({a.y(), b.y(), c.y()});
			int max_x = std::max({a.x(), b.x(), c.x()});
			int max_y = std::max({a.y(), b.y(), c.y()});
			return {{min_x, min_y}, {max_x, max_y}};
		}

	};

	struct Line2d {
		Point2d a, b;

		Line2d() = default;
		Line2d(Point2d  a_, Point2d  b_) : a(std::move(a_)), b(std::move(b_)) {}
		Line2d(const Pixel& a_, const Pixel& b_) : a(pixel_to_point2d(a_)), b(pixel_to_point2d(b_)) {}

		[[nodiscard]] Vector2d direction() const { return b - a; }

		//return 0: on line
		//return 1: on left
		//return -1: on right
		[[nodiscard]] bool check_position(const Point2d& p) const {
			Vector2d pa = p - a, pb = p - b;
			return sign(cross(pa, pb));
		}
	};

}

