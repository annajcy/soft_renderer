#pragma once

#include "bvh.h"
#include "gpu.h"
#include "mesh.h"
#include "shader_raytracer.h"

namespace raytrace {

	using namespace gpu;

	class Raytracer : public GPU {

	private:
		std::vector<std::array<Intermediate_shader_data, 3>> surfaces{};
		std::shared_ptr<Shader> shader{};
		std::shared_ptr<rendering::Camera> camera{};
		std::shared_ptr<rendering::Lighting> lights{};
		std::shared_ptr<BVH_node> bvh_root {nullptr};

	public:

		int max_depth{ 2 };
		decimal reflected_loss { 0.2 };

		Raytracer() = default;

		template<typename T>
		void set_shader(const std::shared_ptr<T> shader_)
		requires Inherited<Shader, typename std::remove_reference<T>::type> {
			shader = shader_;
		}

		void set_camera(const std::shared_ptr<rendering::Camera> &camera_) {
			camera = camera_;
		}

		void set_lights(const std::shared_ptr<rendering::Lighting> &lights_) {
			lights = lights_;
		}

		math::Color_decimal ray_cast_color(const math::Ray &ray, int depth) {
			if (depth > max_depth) return math::Color::black().to_color_decimal();
			auto queried_node = bvh_root->query(ray);

			std::pair<math::RayHit, Triangle> closest_hit{};
			bool hit_found = false;

			for (auto &triangle : queried_node->triangles) {
				auto ray_hit = math::RayHit(triangle.position, ray);
				if (!ray_hit.inside()) continue;

				if (!hit_found || ray_hit.distance < closest_hit.first.distance) {
					closest_hit.first = ray_hit;
					closest_hit.second = triangle;
					hit_found = true;
				}
			}

			if (!hit_found) {
				return math::Color::black().to_color_decimal();
			}

			//reflect
			auto reflected_ray = ray.reflect(closest_hit.first.distance, closest_hit.second.position.normal());
			math::Color_decimal result = reflected_loss * ray_cast_color(reflected_ray, depth + 1);

			auto barycentric = closest_hit.first.barycentric;
			auto tri = surfaces[closest_hit.second.index];

			Intermediate_shader_data interpolated_data = Intermediate_shader_data::interpolate_intermediate_shader_data(
					tri[0], tri[1], tri[2], barycentric
			);

			Fragment_shader_input_data frag{};

			frag.uv = interpolated_data.uv;
			frag.view_position = interpolated_data.view_position;
			frag.view_normal = interpolated_data.view_normal.normalize();

			//tangent
			decimal nx = frag.view_normal.x();
			decimal ny = frag.view_normal.y();
			decimal nz = frag.view_normal.z();

			if (!sign(nx) && !sign(ny)) {
				frag.view_tangent = math::Vector3d {1.0, 0.0, 0.0};
			} else {
				double d = std::sqrt(nx * nx + nz * nz);
				frag.view_tangent = math::Vector3d {(nx * ny) / d, d, (nz * ny) / d}.normalize();
			}

			// process lighting and occlusion
			auto occluded_lights = lights;

			// if light arrives, perform shading
			result += (1.0 - reflected_loss) * shader->fragment_shader(frag, camera, occluded_lights).color;

			return result;
		}

		void draw_model(const std::shared_ptr<mesh::Model> &model) override {

			for (int fid = 0; fid < model->face_count(); fid ++)
			{
				//prepare data
				std::array<Vertex_shader_input_data, 3> face_vs_data{};
				for (int vid : {0, 1, 2}) {
					Vertex_shader_input_data input{};
					input.uv = model->uv(fid, vid);
					input.position = model->position(fid, vid);
					input.normal = model->normal(fid, vid);
					face_vs_data[vid] = input;
				}

				//vertex shade
				std::array<Intermediate_shader_data, 3> intermediate_surface{};
				for (int vid : {0, 1, 2}) {
					intermediate_surface[vid] = shader->vertex_shader(face_vs_data[vid]);
				}

				surfaces.push_back(intermediate_surface);
			}

			std::vector<Triangle> triangles{};
			triangles.reserve(surfaces.size());
			for (int i = 0; i < surfaces.size(); i ++ ) {
				triangles.push_back(Triangle{math::Triangle3d{
					surfaces[i][0].view_position,
					surfaces[i][1].view_position,
					surfaces[i][2].view_position},
											 i});
			}

			bvh_root = std::make_shared<BVH_node>(triangles);
			bvh_root->split(0);

			auto rays = camera->generate_rays();

			for (auto &[ray, pixel] : rays) {
				set_pixel(pixel.x(), pixel.y(), math::Color(ray_cast_color(ray, 0)));
			}

		}

	};

}



