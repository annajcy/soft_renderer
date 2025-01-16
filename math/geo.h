#pragma once

#include <utility>

#include "base.h"
#include "vec.h"
#include "mat.h"
#include "alias.h"

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
			Vector3d ab = b - a, bc = c - b;
			return cross(ab, bc).normalize();
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

		Surface(const Point3d &p_, const Vector3d &normal_) : p(p_), normal(normal_) {}
		Surface(Point3d &&p_, Vector3d &&normal_) : p(std::move(p_)), normal(std::move(normal_)) {}
		explicit Surface(const Triangle3d &abc) : p(abc.a), normal(abc.normal()) {}

		/// side_test. define side where normal points to inside
		/// \return 1 : inside, 0 : on the surface, -1 : out side the surface
		[[nodiscard]] int side_test(const math::Point3d& point) const {
			return sign((point - p).dot(normal));
		}

		/// side_test for triangle
		/// \return 1 : inside, 0 : mixed, -1 : outside the surface
		[[nodiscard]] int side_test(const math::Triangle3d& triangle) const {
			if (side_test(triangle.a) != -1 && side_test(triangle.b) != -1 && side_test(triangle.b) != -1 ) return 1;
			if (side_test(triangle.a) != 1 && side_test(triangle.b) != 1 && side_test(triangle.b) != 1 ) return -1;
			return 0;
		}
	};

	struct Ray {
		Point3d origin{};
		Vector3d direction{};

		Ray() = default;
		Ray(const Point3d &origin_, const Point3d &direction_) : origin(origin_), direction(direction_) { }
		Ray(Point3d &&origin_, Point3d &&direction_) : origin(std::move(origin_)), direction(std::move(direction_)) { }

		[[nodiscard]] Point3d evaluate(decimal t) const {
			return origin + direction * t;
		}

		[[nodiscard]] int intersect_with_sphere(const Sphere &sphere, std::pair<decimal, decimal> &result) const {
			Vector3d co = origin - sphere.origin;
			decimal a = direction.dot(direction);
			decimal b = 2.0 * co.dot(direction);
			decimal c = co.dot(co) - sphere.radius * sphere.radius;
			decimal delta = b * b - 4 * a * c;
			if (sign(delta) == -1) return -1;
			else {
				if (sign(delta) == 0) result = {-b / (2.0 * a), -b / (2.0 * a)};
				else if (sign(delta) == 1) result = { (-b - std::sqrt(delta)) / (2.0 * a), (-b + std::sqrt(delta)) / (2.0 * a)};
				return sign(delta);
			}
		}

		[[nodiscard]] decimal intersect_with_surface(const Surface& surface) const {
			Vector3d op = surface.p - origin;
			return op.dot(surface.normal) / direction.dot(surface.normal);
		}

	};

	struct RayHit {
		Triangle3d surface{};
		Ray ray{};
		decimal t{};
		std::tuple<decimal, decimal, decimal> barycentric{};

		RayHit(const Triangle3d &surface_, const Ray &ray_) : surface(surface_), ray(ray_) {
			evaluate();
		}

		RayHit(Triangle3d &&surface_, Ray &&ray_) : surface(std::move(surface_)), ray(std::move(ray_)) {
			evaluate();
		}

		void evaluate() {
			//Möller Trumbore Algorithm
			Vector3d e1 = surface.b - surface.a;
			Vector3d e2 = surface.c - surface.b;
			Vector3d s0 = ray.origin - surface.a;
			Vector3d s1 = cross(ray.direction, e2);
			Vector3d s2 = cross(s0, e1);
			decimal factor = s1.dot(e1);

			t = s2.dot(e2) / factor;
			auto beta = s1.dot(s0) / factor;
			auto gamma = s2.dot(ray.direction) / factor;
			auto alpha = 1.0 - beta - gamma;
			barycentric = {alpha, beta, gamma};
		}

		[[nodiscard]] bool inside() const {
			auto &[alpha, beta, gamma] = barycentric;
			if (alpha < 0 || alpha > 1) return false;
			if (beta < 0 || beta > 1) return false;
			if (gamma < 0 || gamma > 1) return false;
			return true;
		}

	};

}

