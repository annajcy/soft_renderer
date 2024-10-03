#pragma once

#include "base.h"
#include "geo.h"

namespace math {
    using Homo2d = Vec3f;
	using Homo3d = Vec4f;
    
    inline Homo2d homo_point(decimal x, decimal y) { return Homo2d{x, y, 1.0}; }
	inline Homo3d homo_point(decimal x, decimal y, decimal z) { return Homo3d{x, y, z, 1.0}; } 

	inline Homo2d to_homo_point(const Point2d& v) { return Homo2d{v.x(), v.y(), 1.0}; }
	inline Homo3d to_homo_point(const Point3d& v) { return Homo3d{v.x(), v.y(), v.z(), 1.0}; } 

	inline Point2d to_point(const Homo2d& v) { return Point2d{v.x() / v.z(), v.y() / v.z()}; }
	inline Point3d to_point(const Homo3d& v) { return Point3d{v.x() / v.w(), v.y() / v.w(), v.z() / v.w()}; } 

	inline Homo2d normalize_homo_point(const Homo2d& v) { return Homo2d{v.x() / v.z(), v.y() / v.z(), 1.0}; }
	inline Homo3d normalize_homo_point(const Homo3d& v) { return Homo3d{v.x() / v.w(), v.y() / v.w(), v.z() / v.w(), 1.0}; } 

	inline Homo2d homo_vector(decimal x, decimal y) { return Homo2d{x, y, 0.0}; }
	inline Homo3d homo_vector(decimal x, decimal y, decimal z) { return Homo3d{x, y, z, 0.0}; } 

	inline Homo2d to_homo_vector(const Vector2d& v) { return Homo2d{v.x(), v.y(), 0.0}; }
	inline Homo3d to_homo_vector(const Vector3d& v) { return Homo3d{v.x(), v.y(), v.z(), 0.0}; } 

	using Transform3d = Mat4x4;

	inline Transform3d translate(decimal tx, decimal ty, decimal tz) { 
		return Transform3d{
			{1.0, 0.0, 0.0, tx},
			{0.0, 1.0, 0.0, ty},
			{0.0, 0.0, 1.0, tz},
			{0.0, 0.0, 0.0, 1.0}
		}; 
	}

	inline Transform3d rotate_z(decimal theta) { 
		decimal sin_theta = std::sin(deg_to_rad(theta));
		decimal cos_theta = std::cos(deg_to_rad(theta));
		return Transform3d{
			{cos_theta, -sin_theta, 0.0, 0.0},
			{sin_theta, cos_theta, 0.0, 0.0},
			{0.0, 0.0, 1.0, 0.0},
			{0.0, 0.0, 0.0, 1.0}
		}; 
	}

	inline Transform3d rotate_x(decimal theta) { 
		decimal sin_theta = std::sin(deg_to_rad(theta));
		decimal cos_theta = std::cos(deg_to_rad(theta));
		return Transform3d{
			{1.0, 0.0, 0.0, 0.0},
			{0.0, cos_theta, -sin_theta, 0.0},
			{0.0, sin_theta, cos_theta, 0.0},
			{0.0, 0.0, 0.0, 1.0}
		}; 
	}

	inline Transform3d rotate_y(decimal theta) { 
		decimal sin_theta = std::sin(deg_to_rad(theta));
		decimal cos_theta = std::cos(deg_to_rad(theta));
		return Transform3d{
			{cos_theta, 0.0, sin_theta, 0.0},
			{0.0, 1.0, 0.0, 0.0},
			{-sin_theta, 0.0, cos_theta, 0.0},
			{0.0, 0.0, 0.0, 1.0}
		}; 
	}

	//rotate by axis
	inline Transform3d rotate(const Vector3d& axis, decimal theta) { 
		auto n = axis.normalize();
		decimal x = n.x(), y = n.y(), z = n.z();
		decimal sin_theta = std::sin(deg_to_rad(theta));
		decimal cos_theta = std::cos(deg_to_rad(theta));
		return Transform3d{
			{x * x * (1.0 - cos_theta) + cos_theta, x * y * (1.0 - cos_theta) - z * sin_theta, x * z * (1.0 - cos_theta) + y * sin_theta, 0.0},
			{x * y * (1.0 - cos_theta) + z * sin_theta, y * y * (1.0 - cos_theta) + cos_theta, y * z * (1.0 - cos_theta) - x * sin_theta, 0.0},
			{x * z * (1.0 - cos_theta) - y * sin_theta,  y * z * (1.0 - cos_theta) + x * sin_theta,  z * z * (1.0 - cos_theta) + cos_theta, 0.0},
			{0.0, 0.0, 0.0, 1.0}
		};
	}

	//rotate by orthogonal base
	inline Transform3d rotate(const Vector3d& i, const Vector3d& j, const Vector3d& k) {
		return Transform3d {
			{i.x(), j.x(), k.x(), 0.0},
			{i.y(), j.y(), k.y(), 0.0},
			{i.z(), j.z(), k.z(), 0.0},
			{0.0, 0.0, 0.0, 1.0}
		};
	}

	//scale
	inline Transform3d scale(decimal factor_x, decimal factor_y, decimal factor_z) { 
		return Transform3d{
			{factor_x, 0.0, 0.0, 0.0},
			{0.0, factor_y, 0.0, 0.0},
			{0.0, 0.0, factor_z, 0.0},
			{0.0, 0.0, 0.0, 1.0}
		}; 
	}

	//TODO : TEST
	inline Transform3d mirror(const Vector3d& axis) {
		auto n = axis.normalize();
		decimal x = n.x(), y = n.y(), z = n.z();
		return Transform3d{
        	{2 * x * x - 1.0, 2 * x * y, 2 * x * z, 0.0},
        	{2 * x * y, 2 * y * y - 1.0, 2 * y * z, 0.0},
        	{2 * x * z, 2 * y * z, 2 * z * z - 1.0, 0.0},
        	{0.0, 0.0, 0.0, 1.0}
    	};
	}

	inline Transform3d model(const Transform3d& rotate, const Transform3d& translate) {
		return translate * rotate;
	}

	//assume front and top are normalized vector
	inline Transform3d view(const Vector3d& front, const Vector3d& top, const Vector3d& position) {
		auto right = cross(front, top).normalize();
		auto up = cross(right, front).normalize();
		auto ret = (translate(position.x(), position.y(), position.z()) * rotate(right, up, -front)).inv();
		return ret;
	}

	//far represent the abs value of the coordination of the fear plane
	inline Transform3d projection_orthogonal(decimal near, decimal far, decimal top, decimal bottom, decimal left, decimal right) {
		return scale(2.0 / (right - left), 2.0 / (top - bottom), 2.0 / (near - far)) * 
				translate(-(right + left) * 0.5, -(top + bottom) * 0.5, -(far + near) * 0.5);
	}

	//transform into clip space
	inline Transform3d projection_perspective(decimal fov, decimal aspect_ratio, decimal near, decimal far) {
		auto half_fov = deg_to_rad(fov / 2.0);
		auto top = tan(half_fov) * -near, bottom = -top;
		auto right = aspect_ratio * top, left = -right;

		auto perspective_to_orthogonal = Transform3d {
			{near, 0.0, 0.0, 0.0},
			{0.0, near, 0.0, 0.0},
			{0.0, 0.0, (near + far), - near * far},
			{0.0, 0.0, 1.0, 0.0},
		};

		auto result = projection_orthogonal(near, far, top, bottom, left, right) * perspective_to_orthogonal;
		return result;
	}

	//transform ndc to screen space
	inline Transform3d screen(int width, int height) {
		return translate(width * 0.5 , height * 0.5, 0.0) *
				scale(width * 0.5 , height * 0.5, 1.0);
	}

}