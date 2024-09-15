#pragma once

#include "base.h"
#include "color.h"

namespace math {
	using Pixel = Vec2i;
	using Point2d = Vec2f;
	using Point3d = Vec3f;
	using Vector2d = Vec2f;
	using Vector3d = Vec3f;
	using Homo2d = Vec3f;
	using Homo3d = Vec4f;

	using Pixel_color = std::pair<Pixel, Color>;
	using Pixel_alpha = std::pair<Pixel, decimal>;

	Point2d pixel_to_point2d(const Pixel& point) {
		return Point2d({point.x() + 0.5f, point.y() + 0.5f});
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
			auto min_x = std::min({a.x(), b.x(), c.x()});
			auto min_y = std::min({a.y(), b.y(), c.y()});
			auto max_x = std::max({a.x(), b.x(), c.x()});
			auto max_y = std::max({a.y(), b.y(), c.y()});
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

