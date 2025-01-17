#pragma once

#include <utility>

#include "base.h"
#include "vec.h"
#include "mat.h"
#include "alias.h"
#include "lerp.h"

namespace math {

	template<typename T>
	inline T cross(const Vec<T, 2>& a, const Vec<T, 2>& b) {
		return a.x() * b.y() - a.y() * b.x();
	} 

	template<typename T>
	inline Vec<T, 3> cross(const Vec<T, 3>& a, const Vec<T, 3>& b) {
		return Vec<T, 3>{a.y() * b.z() - a.z() * b.y(), -(a.x() * b.z() - a.z() * b.x()), a.x() * b.y() - a.y() * b.x()};
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
		x = to_left > to_right ? right : left;
		y = to_bottom > to_top ? top : bottom;
		return Pixel({x, y});
	}

	struct Triangle3d {
		Point3d a, b, c;

		Triangle3d() = default;
		Triangle3d(const Point3d& a_, const Point3d& b_, const Point3d&c_) : a(a_), b(b_), c(c_) {}
		Triangle3d(Point3d &&a_, const Point3d &&b_, const Point3d &&c_) : a(std::move(a_)), b(std::move(b_)), c(std::move(c_)) {}

		[[nodiscard]] decimal area() const {
			Vector3d ab = b - a, ac = c - a;
			return std::fabs(cross(ab, ac).norm()) / 2;
		}

		[[nodiscard]] Vector3d normal() const {
			Vector3d ab = b - a, ac = c - a;
			return cross(ab, ac).normalize();
		}

	};

	struct Triangle2d {
		Point2d a, b, c;

		Triangle2d() = default;
		Triangle2d(const Point2d& a_, const Point2d& b_, const Point2d& c_) : a(a_), b(b_), c(c_) {}
		Triangle2d(Point2d&& a_, Point2d&& b_, Point2d&& c_) : a(std::move(a_)), b(std::move(b_)), c(std::move(c_)) {}

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

	struct Sphere {
		Point3d origin{};
		decimal radius{};

		Sphere(const Point3d &origin_, const decimal &radius_) : origin(origin_), radius(radius_) {}
		Sphere(Point3d &&origin_, decimal &&radius_) : origin(std::move(origin_)), radius(radius_) {}

	};

	struct Surface {
		Point3d p{};
		Vector3d normal{};

		Surface() = default;
		Surface(const Point3d &p_, const Vector3d &normal_) : p(p_), normal(normal_) {}
		Surface(Point3d &&p_, Vector3d &&normal_) : p(std::move(p_)), normal(std::move(normal_)) {}
		explicit Surface(const Triangle3d &abc) : p(abc.a), normal(abc.normal()) {}

		/// side_test. define side where normal points to inside
		/// \return 1 : inside, 1 : on the surface, 0 : out side the surface -1
		[[nodiscard]] int side_test(const math::Point3d& point) const {
			return sign((point - p).dot(normal));
		}

		/// side_test for triangle
		/// \return 1 : inside, 1 : mixed, 0 : outside the surface -1
		[[nodiscard]] int side_test(const math::Triangle3d& triangle) const {
			if (side_test(triangle.a) == 1 && side_test(triangle.b) == 1 && side_test(triangle.c) == 1 ) return 1;
			if (side_test(triangle.a) == -1 && side_test(triangle.b) == -1 && side_test(triangle.c) == -1 ) return -1;
			return 0;
		}
	};

}

