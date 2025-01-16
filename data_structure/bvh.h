#pragma once

#include "maths.h"

namespace raytrace {

	struct Triangle {
		math::Triangle3d position{};
		int index{};
	};

	struct AABB {
		math::Point3d origin{};
		math::Vector3d range{};

		/// get_division_axis
		/// \return x : 0, y : 1, z : 2
		[[nodiscard]] int get_division_axis() const {
			if (range.x() >= std::max(range.y(), range.z())) return 0;
			if (range.y() >= std::max(range.x(), range.z())) return 1;
			return 2;
		}

		[[nodiscard]] std::vector<math::Surface> get_surfaces_vec() const {
			return std::vector<math::Surface> {
					get_up_surface(),
					get_bottom_surface(),
					get_left_surface(),
					get_right_surface(),
					get_near_surface(),
					get_far_surface()
			};
		}

		[[nodiscard]] math::Surface get_up_surface() const {
			return math::Surface(up_point(), math::Vector3d{0.0, -1.0, 0.0});
		}

		[[nodiscard]] math::Surface get_bottom_surface() const {
			return math::Surface(origin, math::Vector3d{0.0, 1.0, 0.0});
		}

		[[nodiscard]] math::Surface get_right_surface() const {
			return math::Surface(right_point(), math::Vector3d{-1.0, 0.0, 0.0});
		}

		[[nodiscard]] math::Surface get_left_surface() const {
			return math::Surface(origin, math::Vector3d{1.0, 0.0, 0.0});
		}

		[[nodiscard]] math::Surface get_near_surface() const {
			return math::Surface(near_point(), math::Vector3d{0.0, 0.0, -1.0});
		}

		[[nodiscard]] math::Surface get_far_surface() const {
			return math::Surface(origin, math::Vector3d{0.0, 0.0, 1.0});
		}

		[[nodiscard]] math::Point3d up_point() const {
			return math::Point3d {origin.x(), origin.y() + range.y(), origin.z()};
		}

		[[nodiscard]] math::Point3d right_point() const {
			return math::Point3d {origin.x() + range.x() , origin.y(), origin.z()};
		}

		[[nodiscard]] math::Point3d near_point() const {
			return math::Point3d {origin.x(), origin.y(), origin.z() + range.z()};
		}

		[[nodiscard]] std::pair<AABB, AABB> split_x() const {
			AABB left{}, right{};
			left.range = range;
			left.range.x() /= 2;

			right.range = range;
			right.range.x() /= 2;

			left.origin = origin;
			right.origin = origin;
			right.origin.x() += range.x() / 2;

			return std::make_pair(left, right);
		}

		[[nodiscard]] std::pair<AABB, AABB> split_y() const {
			AABB left{}, right{};
			left.range = range;
			left.range.y() /= 2;

			right.range = range;
			right.range.y() /= 2;

			left.origin = origin;
			right.origin = origin;
			right.origin.y() += range.y() / 2;

			return std::make_pair(left, right);
		}

		[[nodiscard]] std::pair<AABB, AABB> split_z() const {
			AABB left{}, right{};
			left.range = range;
			left.range.z() /= 2;

			right.range = range;
			right.range.z() /= 2;

			left.origin = origin;
			right.origin = origin;
			right.origin.z() += range.z() / 2;

			return std::make_pair(left, right);
		}

	};

	struct BVH_node : std::enable_shared_from_this<BVH_node> {
		int triangle_max_size{5};
		int max_depth{50};
		std::vector<Triangle> triangles{};
		AABB aabb{};

		std::shared_ptr<BVH_node> left_node{nullptr};
		std::shared_ptr<BVH_node> right_node{nullptr};

		BVH_node(int triangle_max_size_, int max_depth_ ) : triangle_max_size(triangle_max_size_), max_depth(max_depth_){}
		BVH_node(const std::vector<Triangle>& triangles_, int triangle_max_size_, int max_depth_ ) : triangles(triangles_), triangle_max_size(triangle_max_size_), max_depth(max_depth_) {
			aabb = get_AABB();
		}

		[[nodiscard]] AABB get_AABB() const {
			decimal min_x = std::numeric_limits<decimal>::infinity();
			decimal min_y = std::numeric_limits<decimal>::infinity();
			decimal min_z = std::numeric_limits<decimal>::infinity();
			decimal max_x = - std::numeric_limits<decimal>::infinity();
			decimal max_y = - std::numeric_limits<decimal>::infinity();
			decimal max_z = - std::numeric_limits<decimal>::infinity();

			auto eval = [&](const math::Point3d& p) {
				min_x = std::min(min_x, p.x());
				max_x = std::max(max_x, p.x());
				min_y = std::min(min_y, p.y());
				max_y = std::max(max_y, p.y());
				min_z = std::min(min_z, p.z());
				max_z = std::max(max_z, p.z());
			};

			for (auto &triangle : triangles) {
				eval(triangle.position.a);
				eval(triangle.position.b);
				eval(triangle.position.c);
			}

			return AABB{
					math::Point3d {min_x, min_y, min_z},
					math::Point3d {max_x - min_x, max_y - min_y, max_z - min_z}
			};
		}

		static bool enclose(const math::Triangle3d &triangle_, const std::vector<math::Surface>& bounding_box) {
			for (auto &surface : bounding_box)
				if (surface.side_test(triangle_) != 1) return false;
			return true;
		}

		[[nodiscard]] bool is_leaf() const {
			return left_node == nullptr && right_node == nullptr;
		}

		[[nodiscard]] bool is_division_needed() const {
			return triangles.size() > triangle_max_size;
		}

		void update_triangles(const std::vector<Triangle> &triangles_) {
			triangles.clear();
			auto surface_vec = aabb.get_surfaces_vec();
			for (auto &triangle : triangles_) {
				if (enclose(triangle.position, surface_vec))
					triangles.push_back(triangle);
			}
		}

		void split(int depth) {
			if (!is_division_needed()) {
				std::cout << "No more division : " << triangles.size() <<  std::endl;
				return;
			}
			if (depth > max_depth) {
				std::cout << "depth maxed up : " << depth << std::endl;
				return;
			}

			std::cout << "depth : " << depth << std::endl;
			std::cout << "division : " << triangles.size() <<  std::endl;

			left_node = std::make_shared<BVH_node>(triangle_max_size, max_depth);
			right_node = std::make_shared<BVH_node>(triangle_max_size, max_depth);

			int split_axis = aabb.get_division_axis();

			if (split_axis == 0) {
				auto [left_aabb, right_aabb] = aabb.split_x();
				//split x
				left_node->aabb = left_aabb;
				right_node->aabb = right_aabb;

			} else if (split_axis == 1) {
				auto [left_aabb, right_aabb] = aabb.split_y();
				//split y
				left_node->aabb = left_aabb;
				right_node->aabb = right_aabb;

			} else {
				auto [left_aabb, right_aabb] = aabb.split_z();
				//split z
				left_node->aabb = left_aabb;
				right_node->aabb = right_aabb;
			}

			left_node->update_triangles(triangles);
			right_node->update_triangles(triangles);

			if (!left_node->triangles.empty()) left_node->split(depth + 1);
			else left_node.reset();
			if (!right_node->triangles.empty()) right_node->split(depth + 1);
			else right_node.reset();
		}

		std::shared_ptr<BVH_node> query(const math::Ray &ray) {
			if (is_leaf()) return shared_from_this();
			decimal t_left{}, t_right{};

			if (left_node == nullptr && right_node != nullptr) {
				return right_node->query(ray);
			}

			if (left_node != nullptr && right_node == nullptr) {
				return left_node->query(ray);
			}

			left_node->has_intersection_with_ray(ray, t_left);
			right_node->has_intersection_with_ray(ray, t_right);

			if (t_left < t_right) {
				std::cout << "left is a closer Hit" << std::endl;
				return left_node->query(ray);
			} else {
				std::cout << "right is a closer Hit" << std::endl;
				return right_node->query(ray);
			}
		}

		bool has_intersection_with_ray(const math::Ray &ray, decimal &t_enter_result) const {
			auto surfaces = aabb.get_surfaces_vec();

			decimal t_min_y = std::min(ray.intersect_with_surface(surfaces[0]), ray.intersect_with_surface(surfaces[1]));
			decimal t_max_y = std::max(ray.intersect_with_surface(surfaces[0]), ray.intersect_with_surface(surfaces[1]));

			decimal t_min_x = std::min(ray.intersect_with_surface(surfaces[2]), ray.intersect_with_surface(surfaces[3]));
			decimal t_max_x = std::max(ray.intersect_with_surface(surfaces[2]), ray.intersect_with_surface(surfaces[3]));

			decimal t_min_z = std::min(ray.intersect_with_surface(surfaces[4]), ray.intersect_with_surface(surfaces[5]));
			decimal t_max_z = std::max(ray.intersect_with_surface(surfaces[4]), ray.intersect_with_surface(surfaces[5]));

			decimal t_enter = std::max({t_min_y, t_min_x, t_min_z});
			decimal t_exit = std::min({t_max_y, t_max_x, t_max_z});

			if (t_enter < t_exit) {
				t_enter_result = t_enter;
				return true;
			}

			t_enter_result = std::numeric_limits<decimal>::infinity();
			return false;
		}
	};

}


