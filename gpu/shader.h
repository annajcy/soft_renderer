#pragma once

#include "base.h"
#include "maths.h"
#include "image.h"
#include "camera.h"
#include "rendering.h"

namespace gpu {

	struct Vertex_shader_input_data {
		math::Point3d position{};
		math::Vector3d normal{};
		math::UV uv{};
		math::Color_decimal base_color{};
	};

	struct Intermediate_shader_data {
		math::Homo3d position{};
		math::Point3d view_position{};
		math::Vector3d view_normal{};
		math::UV uv{};
		math::Color_decimal base_color{};
		decimal depth{};
		decimal inv_w{};

		static Intermediate_shader_data interpolate_intermediate_shader_data(
				const Intermediate_shader_data &u,
				const Intermediate_shader_data &v,
				const std::pair<decimal, decimal> &factor) {

			Intermediate_shader_data output{};

			output.position = math::calculate_weighed(u.position, v.position, factor);
			output.view_position = math::calculate_weighed(u.view_position, v.view_position, factor);
			output.view_normal = math::calculate_weighed(u.view_normal, v.view_normal, factor);
			output.base_color = math::calculate_weighed(u.base_color, v.base_color, factor);
			output.uv = math::calculate_weighed(u.uv, v.uv, factor);
			output.depth =  math::calculate_weighed(u.depth, v.depth, factor);
			output.inv_w = math::calculate_weighed(u.inv_w, v.inv_w, factor);

			return output;

		}

		static Intermediate_shader_data interpolate_intermediate_shader_data(
				const Intermediate_shader_data &a,
				const Intermediate_shader_data &b,
				const Intermediate_shader_data &c,
				const std::tuple<decimal, decimal, decimal> &barycentric) {

			Intermediate_shader_data output{};

			output.position = math::calculate_weighed(a.position, b.position, c.position, barycentric);
			output.view_position = math::calculate_weighed(a.view_position, b.view_position, c.view_position, barycentric);
			output.view_normal = math::calculate_weighed(a.view_normal, b.view_normal, c.view_normal, barycentric);
			output.base_color = math::calculate_weighed(a.base_color, b.base_color, c.base_color, barycentric);
			output.uv = math::calculate_weighed(a.uv, b.uv, c.uv, barycentric);
			output.depth = math::calculate_weighed(a.depth, b.depth, c.depth, barycentric);
			output.inv_w = math::calculate_weighed(a.inv_w, b.inv_w, c.inv_w, barycentric);

			return output;
		}

		Intermediate_shader_data& perspective_divide() {
			decimal w = position.w();
			depth /= w;
			inv_w /= w;
			base_color /= w;
			uv /= w;
			view_normal /= w;
			view_position /= w;
			return *this;
		}

		Intermediate_shader_data& perspective_recover() {
			view_position /= inv_w;
			view_normal /= inv_w;
			uv /= inv_w;
			base_color /= inv_w;
			depth /= inv_w;
			return *this;
		}
	};

	struct Fragment_shader_input_data {
		math::Pixel pixel_position{};
		decimal depth{};
		math::Point3d view_position{};
		math::Point3d view_normal{};
		math::Point3d view_tangent{};
		math::UV uv{};
		math::Color_decimal base_color{};
		decimal transparency{};
	};

	struct Final_shader_data {
		math::Color color{};
		math::Pixel pixel_position{};
		decimal depth{};
	};

	class Shader {
	public:
		virtual Intermediate_shader_data vertex_shader(const Vertex_shader_input_data& input) = 0;
		virtual Final_shader_data fragment_shader(const Fragment_shader_input_data& input) = 0;
	};


	//Simple Blinn-Phong Shader
	class Blinn_Phong_Shader : public Shader {
	public:
		//Uniform
		math::Transform3d model{};
		math::Transform3d view{};
		math::Transform3d projection{};
		std::shared_ptr<rendering::Camera> camera { nullptr };
		std::shared_ptr<rendering::Lighting> lighting { nullptr };
		std::shared_ptr<rendering::Texture_set> textures {nullptr };

		//Variable

		Blinn_Phong_Shader(
				math::Transform3d &&model_,
				math::Transform3d &&view_,
				math::Transform3d &&projection_,
				std::shared_ptr<rendering::Camera> &&camera_,
				std::shared_ptr<rendering::Lighting> &&lighting_ ,
				std::shared_ptr<rendering::Texture_set> &&textures_) :
				Shader(),
				model(std::move(model_)),
				view(std::move(view_)),
				projection(std::move(projection_)),
				camera(std::move(camera_)),
				lighting(std::move(lighting_)),
				textures(std::move(textures_)) { }

		Blinn_Phong_Shader(
				const math::Transform3d &model_,
				const math::Transform3d &view_,
				const math::Transform3d &projection_,
				const std::shared_ptr<rendering::Camera> &camera_,
				const std::shared_ptr<rendering::Lighting> &lighting_ ,
				const std::shared_ptr<rendering::Texture_set> &textures_) :
				Shader(),
				model(model_),
				view(view_),
				projection(projection_),
				camera(camera_),
				lighting(lighting_),
				textures(textures_) { }

		Intermediate_shader_data vertex_shader(const Vertex_shader_input_data& input) override {

			Intermediate_shader_data output{};

			auto mvp = projection * view * model;
			auto mv = view * model;

			output.position = mvp * to_homo_point(input.position);
			output.view_position = to_point((mv * to_homo_point(input.position)));
			output.view_normal = to_vector((mv.transpose().inv() * to_homo_vector(input.normal))).normalize();
			output.uv = input.uv;
			output.base_color = input.base_color;
			output.depth = output.view_position.z();
			output.inv_w = 1.0;

			return output;
		}

		Final_shader_data fragment_shader(const Fragment_shader_input_data& input) override {
			Final_shader_data output{};
			output.pixel_position = input.pixel_position;
			output.depth = abs(input.depth);

			auto main_texture_val = to_vector(textures->get_texture("main")->at_uv_bilinear(input.uv.x(), input.uv.y()).to_color_decimal());

			decimal ka = 0.005, kd = 1.0, ks = 0.7937;
			int kp = 100;
			
			math::Vector3d color{};

			for (auto &al : lighting->ambient_lights) {
				color += ka * al.propagate();
			}

			for (auto &pl : lighting->point_lights) {
				auto view_direction = input.view_position.normalize();
				auto light_direction = (input.view_position - pl.origin).normalize();
				auto half_direction = (view_direction + light_direction).normalize();
				auto distance = input.view_position.norm();
				
				auto diffuse = kd * main_texture_val * pl.propagate(distance) * std::max(0.0, input.view_normal.dot(light_direction));
				auto specular = ks * pl.propagate(distance) * std::max(0.0, std::pow(input.view_normal.dot(half_direction), kp));
				
				color += diffuse + specular;
			}

			for (auto &dl : lighting->directional_lights) {
				auto view_direction = input.view_position.normalize();
				auto light_direction = dl.direction.normalize();
				auto half_direction = (view_direction + light_direction).normalize();

				auto diffuse = kd * main_texture_val * dl.propagate() * std::max(0.0, input.view_normal.dot(light_direction));
				auto specular = ks * dl.propagate() * std::max(0.0, std::pow(input.view_normal.dot(half_direction), kp));
				
				color += diffuse + specular;
			}

			output.color = math::Color(math::Color_decimal {color.x(), color.y(), color.z(), input.transparency});
			return output;
		}
	};


}

