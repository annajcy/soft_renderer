#pragma once

#include "base.h"
#include "vec.h"

namespace math {
	using Pixel = Vec2i;
	using Point2d = Vec2f;
	using Point3d = Vec3f;
	using Vector2d = Vec2f;
	using Vector3d = Vec3f;
	using Homo2d = Vec3f;
	using Homo3d = Vec4f;
 
	Point2d pixel_to_point2d(const Pixel& pixel) {
		return Point2d({pixel.x() + 0.5f, pixel.y() + 0.5f});
	}

	void sample_pixel(std::vector<Point2d>& result, const Pixel& pixel, int scale) {
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

	Pixel point2d_to_pixel(const Point2d& point) {
		auto left = std::floor(point.x()), right = std::floor(point.x() + 1);
		auto buttom = std::floor(point.y()), top = std::floor(point.y() + 1);
		auto to_left = point.x() - left;
		auto to_right = right - point.x();
		auto to_buttom = point.y() - buttom;
		auto to_top = top - point.y();
		int x, y;
		if (to_left > to_right) x = right;
		else x = left;
		if (to_buttom > to_top) y = top;
		else y = buttom;
		return Pixel({x, y});
	}

	template<typename T>
	Point2d rotate(const Point2d& p, T angle) {
		return Point2d(p.x() * cos(angle) + p.y() * sin(angle), -p.x() * sin(angle) + p.y() * cos(angle));
	}

	template<typename T>
	T cross(const Vec<T, 2>& a, const Vec<T, 2>& b) {
		return a.x() * b.y() - a.y() * b.x();
	} 

	struct Triangle2d {
		Point2d a, b, c;

		Triangle2d() {}
		Triangle2d(Point2d a_, Point2d b_, Point2d c_) : a(a_), b(b_), c(c_) {}
		Triangle2d(Pixel a_, Pixel b_, Pixel c_) : a(pixel_to_point2d(a_)), b(pixel_to_point2d(b_)), c(pixel_to_point2d(c_)) {}

		decimal area() {
			Vector2d ab = b - a, ac = c - a;
			return std::fabs(cross(ab, ac)) / 2;
		}

		bool enclose(const Point2d& p) {
			Vector2d pa = a - p, pb = b - p, pc = c - p;
			bool all_positive = cross(pa, pb) > 0 && cross(pb, pc) > 0 && cross(pc, pa) > 0;
			bool all_negative = cross(pa, pb) < 0 && cross(pb, pc) < 0 && cross(pc, pa) < 0;
			return all_positive || all_negative;
		}

		std::pair<Pixel, Pixel> get_AABB() {
			int min_x = std::min({a.x(), b.x(), c.x()});
			int min_y = std::min({a.y(), b.y(), c.y()});
			int max_x = std::max({a.x(), b.x(), c.x()});
			int max_y = std::max({a.y(), b.y(), c.y()});
			return {{min_x, min_y}, {max_x, max_y}};
		}

	};

	struct Line2d {
		Point2d a, b;

		Line2d() {}
		Line2d(Point2d a_, Point2d b_) : a(a_), b(b_) {}
		Line2d(Pixel a_, Pixel b_) : a(pixel_to_point2d(a_)), b(pixel_to_point2d(b_)) {}

		Vector2d direction() {
			return b - a;
		} 

		//return 0: on line
		//return 1: on left
		//return -1: on right
		bool check_position(Point2d p) {
			Vector2d pa = p - a, pb = p - b;
			return sign(cross(pa, pb));
		}
	};

}

