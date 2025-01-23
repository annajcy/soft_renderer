#pragma once

#include "base.h"
#include "maths.h"
#include "image.h"
#include "camera.h"
#include "rendering.h"

namespace raytrace {

	struct Fragment_shader_input_data {
		math::Point3d view_position{};
		math::Point3d view_normal{};
		math::UV uv{};
	};

	struct Final_shader_data {
		math::Vector3d color{};
	};

	class Raytrace_shader {
	public:
		virtual Final_shader_data fragment_shader(const Fragment_shader_input_data& input, const rendering::Material& material) = 0;
	};

	//Simple Blinn-Phong Shader
	class Blinn_phong_shader : public Raytrace_shader {
	public:

		std::shared_ptr<rendering::Scene> scene{};

		Final_shader_data fragment_shader(const Fragment_shader_input_data& input, const rendering::Material& material) override {
			Final_shader_data output{};

			auto color_tex = [&](decimal u, decimal v) {
				return material.get_uv_color("main", u, v);
			};

			auto color_tex_val = color_tex(input.uv.x(), input.uv.y());

			auto normal = input.view_normal;
			auto position = input.view_position;

			math::Vector3d color{};

			decimal ka = 0.0005;

			for (auto &al : scene->lighting->get_lights<rendering::Ambient_light>()) {
				color += ka * al->propagate();
			}

			for (auto &pl : scene->lighting->get_lights<rendering::Point_light>()) {
				auto view_direction = position.normalize();
				auto light_direction = (position - pl->origin).normalize();
				auto half_direction = (view_direction + light_direction).normalize();
				auto distance = position.norm();

				auto diffuse = material.kd * color_tex_val * pl->propagate(distance) * std::max(0.0, normal.dot(light_direction));
				auto specular = material.ks * pl->propagate(distance) * std::max(0.0, std::pow(normal.dot(half_direction), material.specular_exp));

				color += diffuse + specular;
			}

			for (auto &dl : scene->lighting->get_lights<rendering::Directional_light>()) {
				auto view_direction = position.normalize();
				auto light_direction = dl->direction.normalize();
				auto half_direction = (view_direction + light_direction).normalize();

				auto diffuse = material.kd * color_tex_val * dl->propagate() * std::max(0.0, normal.dot(light_direction));
				auto specular = material.ks * dl->propagate() * std::max(0.0, std::pow(normal.dot(half_direction), material.specular_exp));

				color += diffuse + specular;
			}

			output.color = math::Color_decimal {color.x(), color.y(), color.z(), 1.0};
			return output;
		}
	};

}

