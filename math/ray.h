#pragma once

#include "vec.h"
#include "geo.h"
#include "lerp.h"

namespace math {
	struct Ray {

		enum class Intersection_type {
			NONE = -1,
			TANGENT = 0,
			SECANT = 1,
		};

		Point3d origin{};
		Vector3d direction{};

		Ray() = default;

		Ray(const Point3d &origin_, const Point3d &direction_)
				: origin(origin_), direction(direction_.normalize()) {
			if (direction_.norm() == 0) {
				throw std::invalid_argument("Direction vector cannot be zero.");
			}
		}

		Ray(Point3d &&origin_, Point3d &&direction_)
				: origin(std::move(origin_)), direction(direction_.normalize()) {
			if (direction_.norm() == 0) {
				throw std::invalid_argument("Direction vector cannot be zero.");
			}
		}

		[[nodiscard]] Point3d evaluate(decimal t) const {
			return origin + direction * t;
		}

		[[nodiscard]] Intersection_type intersect_with_sphere(const Sphere &sphere, std::pair<decimal, decimal> &result) const {
			Vector3d co = origin - sphere.origin;
			decimal a = direction.dot(direction);
			decimal b = 2.0 * co.dot(direction);
			decimal c = co.dot(co) - sphere.radius * sphere.radius;
			decimal delta = b * b - 4 * a * c;
			if (delta < 0) return Intersection_type::NONE;
			decimal t1 = (-b - std::sqrt(delta)) / (2.0 * a);
			decimal t2 = (-b + std::sqrt(delta)) / (2.0 * a);
			result = delta == 0 ? std::make_pair(t1, t1) : std::make_pair(t1, t2);
			return delta == 0 ? Intersection_type::TANGENT : Intersection_type::SECANT;
		}

		[[nodiscard]] bool intersect_with_surface(const Surface& surface, decimal &t) const {
			decimal denominator = direction.dot(surface.normal);
			if (std::abs(denominator) < eps) {
				t = std::numeric_limits<decimal>::infinity();
				return false;
			}
			t = (surface.p - origin).dot(surface.normal) / denominator;
			return true;
		}
	};

	// Reflect a vector `v` around a normal `n`.
	// Assumes `n` is normalized.
	inline Vector3d reflect(const Vector3d& v, const Vector3d& n) {
		return v - 2.0 * v.dot(n) * n;
	}

	// Refract a vector `v` through a surface with normal `n` and indices of refraction `ior`.
	// Returns true if refraction occurs; false otherwise (total internal reflection).
	inline bool refract(Vector3d& refracted, const Vector3d& v, const Vector3d& n, decimal ior) {
		Vector3d unit_v = v.normalize();
		decimal cos_theta = -unit_v.dot(n); // Angle between the ray and the surface normal.

		// Determine refraction ratio based on the ray's direction.
		decimal eta = cos_theta > 0 ? (1.0 / ior) : ior; // Entering or exiting the medium.
		Vector3d adjusted_normal = cos_theta > 0 ? n : -n;

		decimal sin_theta2 = eta * eta * (1.0 - cos_theta * cos_theta);
		if (sin_theta2 > 1.0) {
			// Total internal reflection
			return false;
		}

		decimal cos_theta2 = sqrt(1.0 - sin_theta2);
		refracted = eta * unit_v + (eta * cos_theta - cos_theta2) * adjusted_normal;
		return true;
	}



	struct RayHit {
		Triangle3d surface{};
		Ray ray{};
		decimal distance{};
		std::tuple<decimal, decimal, decimal> barycentric{};

		RayHit() = default;

		RayHit(const Triangle3d &surface_, const Ray &ray_) : surface(surface_), ray(ray_) {
			evaluate();
		}

		RayHit(Triangle3d &&surface_, Ray &&ray_) : surface(std::move(surface_)), ray(std::move(ray_)) {
			evaluate();
		}

		void evaluate() {
			// Möller-Trumbore Algorithm
			Vector3d e1 = surface.b - surface.a;
			Vector3d e2 = surface.c - surface.a; // Corrected edge vector
			Vector3d s0 = ray.origin - surface.a;
			Vector3d s1 = cross(ray.direction, e2);
			decimal factor = s1.dot(e1);

			// Avoid division by zero or numerical instability
			constexpr decimal epsilon = 1e-8;
			if (std::abs(factor) < epsilon) {
				distance = std::numeric_limits<decimal>::infinity();
				barycentric = {-1.0, -1.0, -1.0};
				return;
			}

			decimal inv_factor = 1.0 / factor; // Avoid repeated division
			Vector3d s2 = cross(s0, e1);

			distance = s2.dot(e2) * inv_factor;
			auto beta = s1.dot(s0) * inv_factor;
			auto gamma = s2.dot(ray.direction) * inv_factor;
			auto alpha = 1.0 - beta - gamma;
			barycentric = {alpha, beta, gamma};
		}

		[[nodiscard]] bool inside() const {
			auto &[alpha, beta, gamma] = barycentric;
			if (distance < 0.0) return false;
			if (alpha < 0.0 || alpha > 1.0) return false;
			if (beta < 0.0 || beta > 1.0) return false;
			if (gamma < 0.0 || gamma > 1.0) return false;
			return true;
		}

	};
}