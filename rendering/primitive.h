#pragma once

#include "mesh.h"

namespace rendering {

	struct AABB {

		enum class Axis {
			X = 0, Y = 1, Z = 2
		};

		math::Point3d p_min{};
		math::Point3d p_max{};

		AABB() {
			p_min = {inf, inf, inf};
			p_max = {neg_inf, neg_inf, neg_inf};
		}

		explicit AABB(const math::Point3d& p) : p_min(p), p_max(p) { }

		AABB(const math::Point3d& p1, const math::Point3d& p2) {
			p_min = math::Point3d{ std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y()), std::min(p1.z(), p2.z()) };
			p_max = math::Point3d{ std::max(p1.x(), p2.x()), std::max(p1.y(), p2.y()), std::max(p1.z(), p2.z()) };
		}

		AABB operator+(const AABB &rhs) const {
			AABB result{};
			result.p_min = math::Point3d{ std::min(this->p_min.x(), rhs.p_min.x()), std::min(this->p_min.y(), rhs.p_min.y()), std::min(this->p_min.z(), rhs.p_min.z()) };
			result.p_max = math::Point3d{ std::max(this->p_max.x(), rhs.p_max.x()), std::max(this->p_max.y(), rhs.p_max.y()), std::max(this->p_max.z(), rhs.p_max.z()) };
			return result;
		}

		AABB& operator+=(const AABB &rhs) {
			this->p_min = math::Point3d{ std::min(this->p_min.x(), rhs.p_min.x()), std::min(this->p_min.y(), rhs.p_min.y()), std::min(this->p_min.z(), rhs.p_min.z()) };
			this->p_max = math::Point3d{ std::max(this->p_max.x(), rhs.p_max.x()), std::max(this->p_max.y(), rhs.p_max.y()), std::max(this->p_max.z(), rhs.p_max.z()) };
			return *this;
		}

		AABB operator+(const math::Point3d &rhs)  const{
			AABB result{};
			result.p_min = math::Point3d{ std::min(this->p_min.x(), rhs.x()), std::min(this->p_min.y(), rhs.y()), std::min(this->p_min.z(), rhs.z()) };
			result.p_max = math::Point3d{ std::max(this->p_max.x(), rhs.x()), std::max(this->p_max.y(), rhs.y()), std::max(this->p_max.z(), rhs.z()) };
			return result;
		}

		AABB operator+=(const math::Point3d &rhs) {
			this->p_min = math::Point3d{ std::min(this->p_min.x(), rhs.x()), std::min(this->p_min.y(), rhs.y()), std::min(this->p_min.z(), rhs.z()) };
			this->p_max = math::Point3d{ std::max(this->p_max.x(), rhs.x()), std::max(this->p_max.y(), rhs.y()), std::max(this->p_max.z(), rhs.z()) };
			return *this;
		}

		[[nodiscard]] Axis get_longest_axis() const {
			decimal max_val = std::max({range().x(), range().y(), range().z()});
			if (range().x() == max_val) return Axis::X;
			if (range().y() == max_val) return Axis::Y;
			return Axis::Z;
		}

		[[nodiscard]] math::Vector3d range() const {
			return p_max - p_min;
		}

		[[nodiscard]] math::Point3d centroid() const {
			return p_min + range() * 0.5;
		}

		[[nodiscard]] bool inside(const math::Point3d &p) const {
			return p.x() <= p_max.x() && p.x() >= p_min.x() &&
			       p.y() <= p_max.y() && p.y() >= p_min.y() &&
			       p.z() <= p_max.z() && p.z() >= p_min.z();
		}

		[[nodiscard]] bool intersect_with_slab(Axis axis, const math::Ray &ray, std::pair<decimal, decimal> &interval) const {
			decimal t1{}, t2{};
			if (axis == Axis::X) {
				if (sign(ray.direction.x()) == 0) return false;
				t1 = (p_min.x() - ray.origin.x()) / ray.direction.x();
				t2 = (p_max.x() - ray.origin.x()) / ray.direction.x();
			} else if (axis == Axis::Y) {
				if (sign(ray.direction.y()) == 0) return false;
				t1 = (p_min.y() - ray.origin.y()) / ray.direction.y();
				t2 = (p_max.y() - ray.origin.y()) / ray.direction.y();
			} else {
				if (sign(ray.direction.z()) == 0) return false;
				t1 = (p_min.z() - ray.origin.z()) / ray.direction.z();
				t2 = (p_max.z() - ray.origin.z()) / ray.direction.z();
			}

			interval.first = std::min(t1, t2);
			interval.second = std::max(t1, t2);
			return true;
		}

		[[nodiscard]] bool intersect_with_ray(const math::Ray& ray, decimal& distance) const {
			using Interval = std::pair<decimal, decimal>;

			auto overlap = [&](const Interval& a, const Interval& b, Interval& result) -> bool {
				decimal t_min = std::max(a.first, b.first);
				decimal t_max = std::min(a.second, b.second);
				if (t_min > t_max) return false;
				result = std::make_pair(t_min, t_max);
				return true;
			};

			Interval interval{neg_inf, inf};

			Interval interval_x{};
			if (intersect_with_slab(Axis::X, ray, interval_x)) {
				if (!overlap(interval, interval_x, interval)) {
					return false;
				}
			}

			Interval interval_y{};
			if (intersect_with_slab(Axis::Y, ray, interval_y)) {
				if (!overlap(interval, interval_y, interval)) {
					return false;
				}
			}

			Interval interval_z{};
			if (intersect_with_slab(Axis::Z, ray, interval_z)) {
				if (!overlap(interval, interval_z, interval)) {
					return false;
				}
			}

			distance = interval.first;

			return true;
		}

	};

	struct Primitive{
		std::shared_ptr<rendering::Material> material{};
		explicit Primitive(const std::shared_ptr<Material> &material_) : material(material_) {}
		virtual AABB get_AABB() = 0;
		virtual bool intersect(const math::Ray &ray, mesh::Vertex &intersection, decimal &dist) = 0;
	};

	struct Triangle : public Primitive {
		mesh::Triangle triangle{};

		explicit Triangle(const mesh::Triangle &triangle_, const std::shared_ptr<Material> &material_) : Primitive(material_), triangle(triangle_){}
		explicit Triangle(mesh::Triangle &&triangle_, const std::shared_ptr<Material> &material_) : Primitive(material_), triangle(std::move(triangle_)) {}

		AABB get_AABB() override {
			return AABB(triangle.a.position, triangle.b.position) + triangle.c.position;
		}

		bool intersect(const math::Ray &ray, mesh::Vertex &intersection, decimal &distance) override {
			auto ray_hit = math::RayHit(math::Triangle3d{triangle.a.position, triangle.b.position, triangle.c.position}, ray);
			if (ray_hit.inside()) {
				intersection = triangle.interpolate(ray_hit.barycentric);
				distance = (ray.origin - intersection.position).norm();
				return true;
			} else return false;
		}
	};

}