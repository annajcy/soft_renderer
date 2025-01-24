#pragma once

#include "primitive.h"

namespace rendering {

	struct Hit_info {
		mesh::Vertex closest_hit_vert{};
		std::shared_ptr<rendering::Material> material{};
		decimal closest_dist{inf};
	};

	struct BVH_node : std::enable_shared_from_this<BVH_node> {
		static int max_primitives_count;
		static int max_depth;

		std::vector<std::shared_ptr<Primitive>> primitives{};
		std::shared_ptr<BVH_node> left_node{nullptr};
		std::shared_ptr<BVH_node> right_node{nullptr};
		int depth{ 0 };
		AABB aabb{};

		BVH_node(std::vector<std::shared_ptr<Primitive>> &&primitives_, int depth_) : primitives(std::move(primitives_)), depth(depth_) {
			aabb = get_AABB();
		}

		BVH_node(const std::vector<std::shared_ptr<Primitive>> &primitives_, int depth_) : depth(depth_) {
			for (auto &p : primitives_) {
				if (p) {
					primitives.push_back(p);
				}
				else throw std::invalid_argument("");

			}
			aabb = get_AABB();
		}

		[[nodiscard]] bool depth_maxed() const {
			return depth > max_depth;
		}

		[[nodiscard]] bool primitive_mined() const {
			return primitives.size() < max_primitives_count;
		}

		[[nodiscard]] bool is_leaf() const {
			return !left_node && !right_node;
		}

		[[nodiscard]] bool is_only_left() const {
			return left_node && !right_node;
		}

		[[nodiscard]] bool is_only_right() const {
			return !left_node && right_node;
		}

		[[nodiscard]] AABB get_AABB() const {
			AABB result{};
			for (auto &p : primitives) {
				result += p->get_AABB();
			}
			return result;
		};

		bool get_closest_primitive(Hit_info &hit_info, const math::Ray &ray, const std::shared_ptr<BVH_node>& queried_node) {

			bool hit_found = false;

			for (auto& pri: queried_node->primitives) {
				mesh::Vertex hit_vert{};
				decimal dist{};
				if (!pri->intersect(ray, hit_vert, dist)) continue;
				if (!hit_found || dist < hit_info.closest_dist) {
					hit_info.closest_hit_vert = hit_vert;
					hit_info.closest_dist = dist;
					hit_info.material = pri->material;
				}

				hit_found = true;
			}

			return hit_found;
		}

		bool query(Hit_info &hit_info, const math::Ray &ray) {
			decimal distance{};
			// Check if the ray intersects the AABB of the current node
			if (!aabb.intersect_with_ray(ray ,distance)) {
				return false; // No intersection with this node
			}

			// If this is a leaf node, check for intersection with its primitives
			if (is_leaf()) {
				return get_closest_primitive(hit_info, ray, shared_from_this());
			}

			bool hit_found = false;

			// Query the left child node if it exists
			if (left_node) {
				Hit_info left_hit_info = hit_info; // Create a local copy to track hits in the left subtree
				if (left_node->query(left_hit_info, ray)) {
					hit_found = true;
					if (left_hit_info.closest_dist < hit_info.closest_dist) {
						hit_info = left_hit_info; // Update the closest hit information
					}
				}
			}

			// Query the right child node if it exists
			if (right_node) {
				Hit_info right_hit_info = hit_info; // Create a local copy to track hits in the right subtree
				if (right_node->query(right_hit_info, ray)) {
					hit_found = true;
					if (right_hit_info.closest_dist < hit_info.closest_dist) {
						hit_info = right_hit_info; // Update the closest hit information
					}
				}
			}

			return hit_found;
		}


		void build() {

			if (depth_maxed()) {
				std::cout << "depth maxed up: " << depth << std::endl;
				return;
			}

			if (primitive_mined()) {
				std::cout << "primitive maxed up: " << primitives.size() << std::endl;
				return;
			}

			std::cout << "depth: " << depth << std::endl;
			std::cout << "primitive: " << primitives.size() << std::endl;

			auto cmp_x = [&](const std::shared_ptr<Primitive> &a, const std::shared_ptr<Primitive> &b) {
				auto center_a = a->get_AABB().centroid();
				auto center_b = b->get_AABB().centroid();
				if (center_a.x() != center_b.x()) return center_a.x() < center_b.x();
				if (center_a.y() != center_b.y()) return center_a.y() < center_b.y();
				if (center_a.z() != center_b.z()) return center_a.z() < center_b.z();
				return true;
			};

			auto cmp_y = [&](const std::shared_ptr<Primitive> &a, const std::shared_ptr<Primitive> &b) {
				auto center_a = a->get_AABB().centroid();
				auto center_b = b->get_AABB().centroid();
				if (center_a.y() != center_b.y()) return center_a.y() < center_b.y();
				if (center_a.z() != center_b.z()) return center_a.z() < center_b.z();
				if (center_a.x() != center_b.x()) return center_a.x() < center_b.x();
				return true;
			};

			auto cmp_z = [&](const std::shared_ptr<Primitive> &a, const std::shared_ptr<Primitive> &b) {
				auto center_a = a->get_AABB().centroid();
				auto center_b = b->get_AABB().centroid();
				if (center_a.z() != center_b.z()) return center_a.z() < center_b.z();
				if (center_a.x() != center_b.x()) return center_a.x() < center_b.x();
				if (center_a.y() != center_b.y()) return center_a.y() < center_b.y();
				return true;
			};

			std::function<bool(std::shared_ptr<Primitive> &, std::shared_ptr<Primitive> &)> cmp{};

			auto axis = aabb.get_longest_axis();

			if (axis == AABB::Axis::X) {
				cmp = cmp_x;
			} else if (axis == AABB::Axis::Y) {
				cmp = cmp_y;
			} else {
				cmp = cmp_z;
			}

			std::sort(primitives.begin(), primitives.end(), cmp);

			std::vector<std::shared_ptr<Primitive>> left_primitive{};
			std::vector<std::shared_ptr<Primitive>> right_primitive{};

			int mid = primitives.size() / 2;

			left_primitive.reserve(mid);
			right_primitive.reserve(primitives.size() - mid);

			for (int i = 0; i < mid; i ++ ) {
				left_primitive.push_back(primitives[i]);
			}

			for (int i = mid; i < primitives.size(); i ++) {
				right_primitive.push_back(primitives[i]);
			}

			if (!left_primitive.empty()) {
				std::cout << "build left" << std::endl;
				left_node = std::make_shared<BVH_node>(left_primitive, depth + 1);
				left_node->build();
			}

			if (!right_primitive.empty()) {
				std::cout << "build right" << std::endl;
				right_node = std::make_shared<BVH_node>(right_primitive, depth + 1);
				right_node->build();
			}
		};

	};

	int BVH_node::max_depth = 50;
	int BVH_node::max_primitives_count = 200;

}

