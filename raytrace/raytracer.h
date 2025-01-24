#pragma once

#include "bvh.h"
#include "gpu.h"
#include "mesh.h"
#include "raytrace_shader.h"

namespace raytrace {

	using namespace gpu;

	class Raytracer : public GPU {
	public:
		std::shared_ptr<rendering::BVH_node> bvh{};
		std::shared_ptr<Raytrace_shader> shader{};

		template<typename T>
		void set_shader(const std::shared_ptr<T>& shader_)
		requires Inherited<Raytrace_shader, typename std::remove_reference<T>::type> {
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

		Raytracer() = default;

		void render_scene(const std::shared_ptr<rendering::Scene>& scene) override {
			std::vector<std::shared_ptr<rendering::Primitive>> primitives;

			// Collect all primitives in the scene
			for (auto& model : scene->models) {
				for (auto& pri : model->primitives) {
					primitives.push_back(pri);
				}
			}

			// Build the BVH
			std::cout << "Building BVH" << std::endl;
			bvh = std::make_shared<rendering::BVH_node>(primitives, 0);
			bvh->build();
			std::cout << "BVH built" << std::endl;

			// Generate rays and compute pixel colors
			auto camera_rays = scene->camera->generate_rays();
			for (auto& [ray, pixel] : camera_rays) {
				auto color = math::Color(to_homo_point(ray_cast_color(ray, 0)));
				std::cout << "Set pixel :" << pixel.x() << " " << pixel.y() << " " << color << "\n";
				set_pixel(pixel.x(), pixel.y(), color);
			}
		}

		math::Vector3d ray_cast_color(const math::Ray& ray, int depth) {
			if (depth > max_depth) return math::Vector3d::zeros();

			decimal dist{};
			rendering::Hit_info hit_info{};

			// Check for intersection with BVH
			if (bvh->aabb.intersect_with_ray(ray, dist)) {
				if (!bvh->query(hit_info, ray)) {
					return math::Vector3d::zeros();
				}
			} else {
				return math::Vector3d::zeros();
			}

			math::Vector3d color = math::Vector3d::zeros();
			auto& [closest_hit_vert, material, closest_dist] = hit_info;
			auto reflected_loss = material->reflection_loss;

			const auto& normal = closest_hit_vert.normal;
			const auto& hit_point = closest_hit_vert.position;


			// Handle material types
			if (material->type == rendering::Material::Material_type::REFLECTION_ONLY) {
				// Reflect the ray
				math::Vector3d reflected_dir = math::reflect(ray.direction, normal);
				math::Ray reflected_ray(hit_point + reflected_dir * 1e-4, reflected_dir);
				color += ray_cast_color(reflected_ray, depth + 1);
			} else if (material->type == rendering::Material::Material_type::DIFFUSE_AND_GLOSSY) {
				// Diffuse reflection and glossy reflection
				math::Vector3d reflected_dir = math::reflect(ray.direction, normal);
				math::Ray reflected_ray(hit_point + reflected_dir * 1e-4, reflected_dir);
				color += (1.0 - reflected_loss) *
				         get_shader()->fragment_shader(
								         Fragment_shader_input_data{
										         hit_point,
										         normal,
										         closest_hit_vert.uv},
								         *material)
						         .color;
				color += reflected_loss * ray_cast_color(reflected_ray, depth + 1);
			} else if (material->type == rendering::Material::Material_type::REFRACTION_ONLY) {
				// Refraction and total internal reflection
				math::Vector3d refracted_ray_dir{};
				if (math::refract(refracted_ray_dir, ray.direction , normal, material->ior)) {
					math::Ray refracted_ray{hit_point + refracted_ray_dir * 1e-4, refracted_ray_dir};
					color += ray_cast_color(refracted_ray, depth + 1);
				} else {
					math::Vector3d reflected_dir = math::reflect(ray.direction, normal);
					math::Ray reflected_ray(hit_point + reflected_dir * 1e-4, reflected_dir);
					color += ray_cast_color(reflected_ray, depth + 1);
				}
			}

			return color;
		}
	};

}