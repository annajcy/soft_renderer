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

		[[nodiscard]] math::Surface split_x() const {
			math::Point3d point{origin.x() + range.x() / 2, origin.y(), origin.z()};
			math::Vector3d normal{1.0, 0.0, 0.0};
			return math::Surface{point, normal};
		}

		[[nodiscard]] math::Surface split_y() const {
			math::Point3d point{origin.x(), origin.y() + range.y() / 2, origin.z()};
			math::Vector3d normal{0.0, 1.0, 0.0};
			return math::Surface{point, normal};
		}

		[[nodiscard]] math::Surface split_z() const {
			math::Point3d point{origin.x(), origin.y(), origin.z() + range.z() / 2};
			math::Vector3d normal{0.0, 0.0, 1.0};
			return math::Surface{point, normal};
		}

	};

	struct BVH_node : std::enable_shared_from_this<BVH_node> {
		static int triangle_max_size;
		static int max_depth;
		std::vector<Triangle> triangles{};
		AABB aabb{};

		std::shared_ptr<BVH_node> left_node{nullptr};
		std::shared_ptr<BVH_node> right_node{nullptr};

		BVH_node()= default;
		explicit BVH_node(const std::vector<Triangle>& triangles_) : triangles(triangles_) {
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
					math::Point3d {min_x - eps, min_y - eps, min_z - eps},
					math::Point3d {max_x - min_x + eps, max_y - min_y + eps, max_z - min_z + eps}
			};
		}

		[[nodiscard]] bool is_leaf() const {
			return left_node == nullptr && right_node == nullptr;
		}

		[[nodiscard]] bool is_division_needed() const {
			return triangles.size() > triangle_max_size;
		}

		static void split_triangles(
				const std::shared_ptr<BVH_node> &left,
				const std::shared_ptr<BVH_node> &right,
				const std::vector<Triangle> &triangles,
				const math::Surface &surface) {

			left->triangles.clear();
			right->triangles.clear();

			for (auto &tri : triangles) {
				int side_test = surface.side_test(tri.position);
				if (side_test == 1) {
					left->triangles.push_back(tri);
				} else if (side_test == -1) {
					right->triangles.push_back(tri);
				} else {
					left->triangles.push_back(tri);
					right->triangles.push_back(tri);
				}
			}

			left->aabb = left->get_AABB();
			right->aabb = right->get_AABB();
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

			left_node = std::make_shared<BVH_node>();
			right_node = std::make_shared<BVH_node>();

			int split_axis = aabb.get_division_axis();
			math::Surface split_surface{};

			if (split_axis == 0) {
				split_surface = aabb.split_x();
			} else if (split_axis == 1) {
				split_surface = aabb.split_y();
			} else {
				split_surface = aabb.split_z();
			}

			split_triangles(left_node, right_node, triangles, split_surface);

			if (!left_node->triangles.empty()) left_node->split(depth + 1);
			else left_node.reset();
			if (!right_node->triangles.empty()) right_node->split(depth + 1);
			else right_node.reset();
		}

		std::shared_ptr<BVH_node> query(const math::Ray &ray) {
			if (is_leaf()) return shared_from_this();

			if (left_node == nullptr && right_node != nullptr) {
				return right_node->query(ray);
			}

			if (left_node != nullptr && right_node == nullptr) {
				return left_node->query(ray);
			}

			decimal t_left{}, t_right{};
			left_node->has_intersection_with_ray(ray, t_left);
			right_node->has_intersection_with_ray(ray, t_right);

			if (t_left < t_right) {
				return left_node->query(ray);
			} else {
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

			if (sign(t_enter - t_exit) < 0) {
				t_enter_result = t_enter;
				return true;
			}

			t_enter_result = std::numeric_limits<decimal>::infinity();
			return false;
		}
	};

	int BVH_node::max_depth = 50;
	int BVH_node::triangle_max_size = 200;

}


