#pragma once

#include "base.h"
#include "maths.h"
#include "image.h"
#include "camera.h"
#include "rendering.h"

namespace raytrace {

	struct Vertex_shader_input_data {
		math::Point3d position{};
		math::Vector3d normal{};
		math::UV uv{};
	};

	struct Intermediate_shader_data {
		math::Point3d view_position{};
		math::Vector3d view_normal{};
		math::UV uv{};

		static Intermediate_shader_data interpolate_intermediate_shader_data(
				const Intermediate_shader_data &u,
				const Intermediate_shader_data &v,
				const std::pair<decimal, decimal> &factor) {

			Intermediate_shader_data output{};

			output.view_position = math::calculate_weighed(u.view_position, v.view_position, factor);
			output.view_normal = math::calculate_weighed(u.view_normal, v.view_normal, factor);
			output.uv = math::calculate_weighed(u.uv, v.uv, factor);

			return output;

		}

		static Intermediate_shader_data interpolate_intermediate_shader_data(
				const Intermediate_shader_data &a,
				const Intermediate_shader_data &b,
				const Intermediate_shader_data &c,
				const std::tuple<decimal, decimal, decimal> &barycentric) {

			Intermediate_shader_data output{};

			output.view_position = math::calculate_weighed(a.view_position, b.view_position, c.view_position, barycentric);
			output.view_normal = math::calculate_weighed(a.view_normal, b.view_normal, c.view_normal, barycentric);
			output.uv = math::calculate_weighed(a.uv, b.uv, c.uv, barycentric);

			return output;
		}
	};

	struct Fragment_shader_input_data {
		math::Point3d view_position{};
		math::Point3d view_normal{};
		math::Point3d view_tangent{};
		math::UV uv{};
	};

	struct Final_shader_data {
		math::Color_decimal color{};
	};

	class Shader {
	public:
		virtual Intermediate_shader_data vertex_shader(const Vertex_shader_input_data& input) = 0;
		virtual Final_shader_data fragment_shader(
				const Fragment_shader_input_data& input,
				const std::shared_ptr<rendering::Camera> &camera,
				const std::shared_ptr<rendering::Lighting> &lighting) = 0;
	};

	//Simple Blinn-Phong Shader
	class Blinn_Phong_Shader : public Shader {
	public:
		//Uniform
		math::Transform3d model{};
		math::Transform3d view{};
		std::shared_ptr<rendering::Texture_set> textures {nullptr };

		const int kp = 50;
		const decimal ka = 0.01, kd = 1.0, ks = 0.7937;

		bool use_displacement_texture = true;
		const decimal kh = 2.0, kn = 1.0;

		Blinn_Phong_Shader(
				math::Transform3d &&model_,
				math::Transform3d &&view_,
				std::shared_ptr<rendering::Texture_set> &&textures_) :
				Shader(),
				model(std::move(model_)),
				view(std::move(view_)),
				textures(std::move(textures_)) { }

		Blinn_Phong_Shader(
				const math::Transform3d &model_,
				const math::Transform3d &view_,
				const std::shared_ptr<rendering::Texture_set> &textures_) :
				Shader(),
				model(model_),
				view(view_),
				textures(textures_) { }

		Intermediate_shader_data vertex_shader(const Vertex_shader_input_data& input) override {

			Intermediate_shader_data output{};

			auto mv = view * model;
			output.view_position = to_point((mv * to_homo_point(input.position)));
			output.view_normal = to_vector((mv.transpose().inv() * to_homo_vector(input.normal))).normalize();
			output.uv = input.uv;

			return output;
		}

		Final_shader_data fragment_shader(const Fragment_shader_input_data& input, const std::shared_ptr<rendering::Camera> &camera, const std::shared_ptr<rendering::Lighting> &lighting) override {
			Final_shader_data output{};

			auto color_tex = [&](decimal u, decimal v) {
				return to_vector(textures->get_texture("main")->at_uv_bilinear(u, v).to_color_decimal());
			};

			auto height_tex = [&](decimal u, decimal v) {
				return to_vector(textures->get_texture("height")->at_uv_bilinear(u, v).to_color_decimal());
			};

			auto height_tex_du = [&](decimal u, decimal v) {
				return to_vector(textures->get_texture("height")->at_uv_bilinear_du(u, v).to_color_decimal());
			};

			auto height_tex_dv = [&](decimal u, decimal v) {
				return to_vector(textures->get_texture("height")->at_uv_bilinear_dv(u, v).to_color_decimal());
			};

			auto color_tex_val = color_tex(input.uv.x(), input.uv.y());
			auto height_tex_val = height_tex(input.uv.x(), input.uv.y());

			auto normal = input.view_normal;
			auto position = input.view_position;

			if (use_displacement_texture) {
				auto du = kh * kn * (height_tex_du(input.uv.x(), input.uv.y()).norm() - height_tex_val.norm());
				auto dv = kh * kn * (height_tex_dv(input.uv.x(), input.uv.y()).norm() - height_tex_val.norm());

				auto binormal = cross(input.view_normal, input.view_tangent);
				auto tbn = math::Mat3x3::to_matrix(std::vector<math::Vector3d>{input.view_tangent, binormal, input.view_normal});
				auto local_normal = math::Vector3d {-du, -dv, 1.0}.normalize();

				normal = (tbn * local_normal).normalize();
				position = input.view_position + kn * normal * height_tex_val.norm();
			}

			math::Vector3d color{};

			for (auto &al : lighting->get_lights<rendering::Ambient_light>()) {
				color += ka * al->propagate();
			}

			for (auto &pl : lighting->get_lights<rendering::Point_light>()) {
				auto view_direction = position.normalize();
				auto light_direction = (position - pl->origin).normalize();
				auto half_direction = (view_direction + light_direction).normalize();
				auto distance = position.norm();

				auto diffuse = kd * color_tex_val * pl->propagate(distance) * std::max(0.0, normal.dot(light_direction));
				auto specular = ks * pl->propagate(distance) * std::max(0.0, std::pow(normal.dot(half_direction), kp));

				color += diffuse + specular;
			}

			for (auto &dl : lighting->get_lights<rendering::Directional_light>()) {
				auto view_direction = position.normalize();
				auto light_direction = dl->direction.normalize();
				auto half_direction = (view_direction + light_direction).normalize();

				auto diffuse = kd * color_tex_val * dl->propagate() * std::max(0.0, normal.dot(light_direction));
				auto specular = ks * dl->propagate() * std::max(0.0, std::pow(normal.dot(half_direction), kp));

				color += diffuse + specular;
			}

			output.color = math::Color_decimal {color.x(), color.y(), color.z(), 1.0};
			return output;
		}
	};

}

