#pragma once

#include "base.h"
#include "vec.h"
#include "mat.h"

namespace math {

	using Pixel = Vec2i;
	using Point2d = Vec2f;
	using Point3d = Vec3f;
	using Vector2d = Vec2f;
	using Vector3d = Vec3f;
	using Homo2d = Vec3f;
	using Homo3d = Vec4f;
	using UV = Vec2f;

	inline Homo2d homo_point(decimal x, decimal y) { return Homo2d{x, y, 1.0}; }
	inline Homo3d homo_point(decimal x, decimal y, decimal z) { return Homo3d{x, y, z, 1.0}; } 

	inline Homo2d homo_vector(decimal x, decimal y) { return Homo2d{x, y, 0.0}; }
	inline Homo3d homo_vector(decimal x, decimal y, decimal z) { return Homo3d{x, y, z, 0.0}; } 

	using Transform2d = Mat3x3;
	using Transform3d = Mat4x4;

	inline Transform2d translate(decimal tx, decimal ty) { 
		return Transform2d{
			{1.0, 0.0, tx},
			{0.0, 1.0, ty},
			{0.0, 0.0, 1.0}
		}; 
	}

	inline Transform3d translate(decimal tx, decimal ty, decimal tz) { 
		return Transform3d{
			{1.0, 0.0, 0.0, tx},
			{0.0, 1.0, 0.0, ty},
			{0.0, 0.0, 1.0, tz},
			{0.0, 0.0, 0.0, 1.0}
		}; 
	}

	inline Transform2d rotate(decimal theta) { 
		decimal sin_theta = std::sin(deg_to_rad(theta));
		decimal cos_theta = std::cos(deg_to_rad(theta));
		return Transform2d{
			{cos_theta, -sin_theta, 0.0},
			{sin_theta, cos_theta, 0.0},
			{0.0, 0.0, 1.0}
		}; 
	}

	inline Transform3d rotate_xy(decimal theta) { 
		decimal sin_theta = std::sin(deg_to_rad(theta));
		decimal cos_theta = std::cos(deg_to_rad(theta));
		return Transform3d{
			{cos_theta, -sin_theta, 0.0, 0.0},
			{sin_theta, cos_theta, 0.0, 0.0},
			{0.0, 0.0, 1.0, 0.0},
			{0.0, 0.0, 0.0, 1.0}
		}; 
	}

	inline Transform3d rotate_yz(decimal theta) { 
		decimal sin_theta = std::sin(deg_to_rad(theta));
		decimal cos_theta = std::cos(deg_to_rad(theta));
		return Transform3d{
			{1.0, 0.0, 0.0, 0.0},
			{0.0, cos_theta, -sin_theta, 0.0},
			{0.0, sin_theta, cos_theta, 0.0},
			{0.0, 0.0, 0.0, 1.0}
		}; 
	}

	inline Transform3d rotate_zx(decimal theta) { 
		decimal sin_theta = std::sin(deg_to_rad(theta));
		decimal cos_theta = std::cos(deg_to_rad(theta));
		return Transform3d{
			{cos_theta, 0.0, sin_theta, 0.0},
			{0.0, 1.0, 0.0, 0.0},
			{-sin_theta, 0.0, cos_theta, 0.0},
			{0.0, 0.0, 0.0, 1.0}
		}; 
	}

	inline Transform3d rotate(const Vector3d& axis, decimal theta) { 
		auto n = axis.normalize();
		auto x = n.x(), y = n.y(), z = n.z();
		decimal sin_theta = std::sin(deg_to_rad(theta));
		decimal cos_theta = std::cos(deg_to_rad(theta));
		return Transform3d{
			{x * x * (1.0 - cos_theta) + cos_theta, x * y * (1.0 - cos_theta) - z * sin_theta, x * z * (1.0 - cos_theta) + y * sin_theta, 0.0},
			{x * y * (1.0 - cos_theta) + z * sin_theta, y * y * (1.0 - cos_theta) + cos_theta, y * z * (1.0 - cos_theta) - x * sin_theta, 0.0},
			{x * z * (1.0 - cos_theta) - y * sin_theta,  y * z * (1.0 - cos_theta) + x * sin_theta,  z * z * (1.0 - cos_theta) + cos_theta, 0.0},
			{0.0, 0.0, 0.0, 1.0}
		};
	}

	inline Transform2d scale(decimal factor_x, decimal factor_y) { 
		return Transform2d{
			{factor_x, 0.0, 0.0},
			{0.0, factor_y, 0.0},
			{0.0, 0.0, 1.0}
		}; 
	}

	inline Transform3d scale(decimal factor_x, decimal factor_y, decimal factor_z) { 
		return Transform3d{
			{factor_x, 0.0, 0.0, 0.0},
			{0.0, factor_y, 0.0, 0.0},
			{0.0, 0.0, factor_z, 0.0},
			{0.0, 0.0, 0.0, 1.0}
		}; 
	}

	//direction : 0 -> horizontal, 1 -> vertical
	inline Transform2d shear(decimal factor, bool direction) {
		if (!direction) {
			return Transform2d{
				{1.0, factor, 0.0},
				{0.0, 1.0, 0.0},
				{0.0, 0.0, 1.0}
			}; 
		} else {
			return Transform2d{
				{1.0, 0.0, 0.0},
				{factor, 1.0, 0.0},
				{0.0, 0.0, 1.0}
			}; 
		}
	}

	inline Transform2d mirror(const Vector2d& axis) {
		auto n = axis.normalize();
		auto x = n.x(), y = n.y();
		return Transform2d{
			{2 * x * x - 1.0, 2.0 * x * y, 0.0},
			{2.0 * x * y, 2 * y * y - 1.0, 0.0},
			{0.0, 0.0, 1.0}
		}; 
	}

	inline Transform3d mirror(const Vector3d& normal) {
		auto n = normal.normalize();
		auto x = n.x(), y = n.y(), z = n.z();
		return Transform3d{
        	{2 * x * x - 1.0, 2 * x * y, 2 * x * z, 0.0},
        	{2 * x * y, 2 * y * y - 1.0, 2 * y * z, 0.0},
        	{2 * x * z, 2 * y * z, 2 * z * z - 1.0, 0.0},
        	{0.0, 0.0, 0.0, 1.0}
    	};
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
		return Point2d({pixel.x() + 0.5f, pixel.y() + 0.5f});
	}

	inline Pixel point2d_to_pixel(const Point2d& point) {
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
	inline T cross(const Vec<T, 2>& a, const Vec<T, 2>& b) {
		return a.x() * b.y() - a.y() * b.x();
	} 

	template<typename T>
	inline Vec<T, 3> cross(const Vec<T, 3>& a, const Vec<T, 3>& b) {
		return Vec<T, 3>{a.y() * b.z() - a.z() * b.y(), -(a.x() * b.z() - a.z() * b.x()), a.x() * b.y() - a.y() * b.x()};
	}

	struct Triangle2d {
		Point2d a, b, c;

		Triangle2d() {}
		Triangle2d(Point2d a_, Point2d b_, Point2d c_) : a(a_), b(b_), c(c_) {}
		Triangle2d(Pixel a_, Pixel b_, Pixel c_) : a(pixel_to_point2d(a_)), b(pixel_to_point2d(b_)), c(pixel_to_point2d(c_)) {}

		decimal area() const {
			Vector2d ab = b - a, ac = c - a;
			return std::fabs(cross(ab, ac)) / 2;
		}

		bool enclose(const Point2d& p) const {
			Vector2d pa = a - p, pb = b - p, pc = c - p;
			bool all_positive = cross(pa, pb) > 0 && cross(pb, pc) > 0 && cross(pc, pa) > 0;
			bool all_negative = cross(pa, pb) < 0 && cross(pb, pc) < 0 && cross(pc, pa) < 0;
			return all_positive || all_negative;
		}

		std::pair<Pixel, Pixel> bounding_box() const {
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

		Vector2d direction() const { return b - a; } 

		//return 0: on line
		//return 1: on left
		//return -1: on right
		bool check_position(Point2d p) const {
			Vector2d pa = p - a, pb = p - b;
			return sign(cross(pa, pb));
		}
	};

}

