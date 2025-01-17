#pragma once

#include "vec.h"
#include "geo.h"
#include "lerp.h"

namespace math {
	struct Ray {
		Point3d origin{};
		Vector3d direction{};

		Ray() = default;
		Ray(const Point3d &origin_, const Point3d &direction_) : origin(origin_), direction(direction_.normalize()) { }
		Ray(Point3d &&origin_, Point3d &&direction_) : origin(std::move(origin_)), direction(std::move(direction_.normalize())) { }

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

		[[nodiscard]] Ray reflect(const decimal &distance, const math::Vector3d &normal) const {
			return Ray{ evaluate(distance), 2 * (direction - direction.project_to(normal)) - direction };
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