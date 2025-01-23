#pragma once

#include "bvh.h"
#include "gpu.h"
#include "mesh.h"
#include "raytrace_shader.h"

namespace raytrace
{

	using namespace gpu;

	class Raytracer : public GPU {
	public:

		std::shared_ptr<rendering::BVH_node> bvh{};
		std::shared_ptr<Raytrace_shader> shader{};

		template<typename T>
		void set_shader(const std::shared_ptr<T>& shader_) requires Inherited<Raytrace_shader, typename std::remove_reference<T>::type> {
			shader = dynamic_pointer_cast<Raytrace_shader>(shader_);
		}

		template<typename T = Raytrace_shader>
		std::shared_ptr<T> get_shader() {
			auto result = std::dynamic_pointer_cast<T>(shader);
			if (!result) {
				throw std::runtime_error("Failed to cast shader to the requested type");
			}
			return result;
		}

		int max_depth{2};
		decimal reflected_loss{0.2};

		Raytracer() = default;

		void render_scene(const std::shared_ptr<rendering::Scene>& scene) override
		{
			std::vector<std::shared_ptr<rendering::Primitive>> primitives{};

			for (auto &model : scene->models) {
				for (auto &pri : model->primitives) {
					primitives.push_back(pri);
				}
			}

			std::cout << "Building BVH" << std::endl;

			bvh = std::make_shared<rendering::BVH_node>(primitives, 0);
			bvh->build();

			std::cout << "BVH built" << std::endl;

			auto camera_rays = scene->camera->generate_rays();

			for (auto &[ray, pixel] : camera_rays) {
				//std::cout << "Set pixel :" << pixel.x() << " " << pixel.y() << "\n";
				set_pixel(pixel.x(), pixel.y(), math::Color(to_homo_point(ray_cast_color(ray, 0))));
			}

		}

		math::Vector3d ray_cast_color(const math::Ray& ray, int depth)
		{
			if (depth > max_depth) return math::Vector3d::zeros();

			std::shared_ptr<rendering::BVH_node> queried_node = nullptr;

			decimal dist{};
			if (bvh->aabb.intersect_with_ray(ray, dist)) {
				queried_node = bvh->query(ray);
			}

			if (queried_node == nullptr) {
				return math::Vector3d::zeros();
			}

			mesh::Vertex closest_hit_vert{};
			std::shared_ptr<rendering::Material> material{};
			decimal closest_dist = inf;
			bool hit_found = false;

			for (auto& pri: queried_node->primitives) {
				mesh::Vertex hit_vert{};
				decimal dist{};
				if (!pri->intersect(ray, hit_vert, dist)) continue;
				if (!hit_found || dist < closest_dist)
					closest_hit_vert = hit_vert, closest_dist = dist, material = pri->material;
				hit_found = true;
			}

			if (!hit_found) {
				return math::Vector3d::zeros();
			}

			math::Vector3d color = math::Vector3d::zeros();

			if (material->type == rendering::Material::Material_type::REFLECTION_ONLY) {
				auto reflected_ray = ray.reflect(closest_dist, closest_hit_vert.normal);
				color += ray_cast_color(reflected_ray, depth + 1);
			} else if (material->type == rendering::Material::Material_type::DIFFUSE_AND_GLOSSY) {
				auto reflected_ray = ray.reflect(closest_dist, closest_hit_vert.normal);
				color +=
						//(1.0 - reflected_loss) *
				         get_shader()->fragment_shader(Fragment_shader_input_data{closest_hit_vert.position, closest_hit_vert.normal, closest_hit_vert.uv}, *material).color;
//				color += reflected_loss *
//						ray_cast_color(reflected_ray, depth + 1);
			} else if (material->type == rendering::Material::Material_type::REFRACTION_ONLY) {
				auto reflected_ray = ray.reflect(closest_dist, closest_hit_vert.normal);
				math::Ray refracted_ray{};
				if (ray.refract(refracted_ray, closest_dist, closest_hit_vert.normal, material->ior))
					color += ray_cast_color(refracted_ray, depth + 1);
				else color += ray_cast_color(reflected_ray, depth + 1);
			}

			return color;
		};

	};


}