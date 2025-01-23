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

		[[nodiscard]] Ray reflect(const decimal &distance, const Vector3d &normal) const {
			Vector3d reflected_dir = direction - 2 * direction.dot(normal) * normal;
			return Ray{ evaluate(distance), reflected_dir.normalize() };
		}


		[[nodiscard]] bool refract(Ray& ray, const decimal& distance, const Vector3d& normal, const decimal ior) const {
			// Calculate the dot product of direction and normal
			decimal cos_dn = direction.dot(normal);
			decimal eta = (cos_dn > 0) ? ior : 1.0 / ior; // Determine refractive index ratio based on entry or exit
			Vector3d adjusted_normal = (cos_dn > 0) ? -normal : normal; // Adjust normal for exit scenario
			cos_dn = std::abs(cos_dn);

			// Calculate sin²(t) using Snell's law: sin²(t) = eta² * (1 - cos²(i))
			decimal sin2_t = eta * eta * (1.0 - cos_dn * cos_dn);

			// Check for total internal reflection
			if (sin2_t > 1.0) {
				std::cerr << "Total internal reflection occurs\n";
				return false;
			}

			// Calculate cos(t) from sin²(t): cos²(t) = 1 - sin²(t)
			decimal cos_t = std::sqrt(1.0 - sin2_t);

			// Calculate the refracted direction
			Vector3d refracted_dir = eta * direction + (eta * cos_dn - cos_t) * adjusted_normal;

			// Create the refracted ray
			ray = Ray{ evaluate(distance), refracted_dir.normalize() };

			return true;
		}
	};

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