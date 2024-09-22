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

	inline Transform3d model(const Transform3d& rotate, const Transform3d& translate) {
		return translate * rotate;
	}

	inline Transform3d veiw(const Transform3d& rotate, const Transform3d& translate) {
		return rotate.inv() * translate.inv();
	}

	//assume front and top are normalized vector
	inline Transform3d veiw(const Vector3d& front, const Vector3d& top, const Vector3d& position) {
		auto right = cross(front, top).normalize();
		auto up = cross(right, front).normalize();
		return Transform3d{
			{right.x(), right.y(), right.z(), -right.dot(position)},
			{up.x(), up.y(), up.z(), -up.dot(position)},
			{-front.x(), -front.y(), -front.z(), front.dot(position)},
			{0.0, 0.0, 0.0, 1.0}
		};
	}

	//far repesent the abs value of the coordination of the fear plane
	inline Transform3d projection_orthogonal(decimal near, decimal far, decimal top, decimal bottom, decimal left, decimal right) {
		return Transform3d{
			{2.0 / (right - left), 0.0, 0.0, -(right + left) / (right - left)},
			{0.0, 2.0 / (top - bottom), 0.0, -(top + bottom) / (top - bottom)},
			{0.0, 0.0, 2.0 / (far - near), -(far + near) / (far - near)},
			{0.0, 0.0, 0.0, 1.0}
		};
	}

	//tranform into clip space
	inline Transform3d projection_perspective(decimal near, decimal far, decimal top, decimal bottom, decimal left, decimal right) {
		return Transform3d{
			{2.0 * near/ (right - left), 0.0, (right + left) / (right - left), 0.0},
			{0.0, 2.0 * near / (top - bottom), (top + bottom) / (top - bottom), 0.0},
			{0.0, 0.0, -(far + near) / (far - near), -2.0 * far * near / (far - near)},
			{0.0, 0.0, -1.0, 0.0}
		};
	}

	//tranform into clip space
	inline Transform3d projection_perspective(decimal fov, decimal aspect_ratio, decimal near, decimal far) {
		auto half_fov = deg_to_rad(fov / 2.0);
		return Transform3d{
			{1.0 / aspect_ratio * tan(half_fov), 0.0, 0.0, 0.0},
			{0.0, 1.0 / tan(half_fov), 0.0, 0.0},
			{0.0, 0.0, -(far + near) / (far - near), -2.0 * far * near / (far - near)},
			{0.0, 0.0, -1.0, 0.0}
		};
	}

	//tranform ndc to screen space
	inline Transform3d screen(int width, int height) {
		return Transform3d{
			{width / 2.0, 0.0, 0.0, width / 2.0},
			{0.0, height / 2.0, 0.0, height / 2.0},
			{0.0, 0.0, 0.5, 0.5},
			{0.0, 0.0, 0.0, 1.0}
		};
	}

	
}