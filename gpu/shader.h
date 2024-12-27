#pragma once

#include "base.h"
#include "maths.h"
#include "image.h"
#include "camera.h"
#include "rendering.h"

namespace gpu {

	struct Vertex_shader_input_data {
		math::Homo3d view_position{};

		Vertex_shader_input_data() = default;
		explicit Vertex_shader_input_data(const math::Homo3d &view_position_) :
		view_position(view_position_) {}
	};

	struct Vertex_shader_output_data {
		decimal inv_w{ 1.0 };
		math::Homo3d clip_position{};

		Vertex_shader_output_data() = default;
		explicit Vertex_shader_output_data(const decimal &inv_w_, const math::Homo3d &clip_position_) :
		inv_w(inv_w_), clip_position(clip_position_) {}
	};

	struct Fragment_shader_input_data {
		math::Pixel pixel{};
		decimal depth{};

		Fragment_shader_input_data() = default;
		explicit Fragment_shader_input_data(const math::Pixel  &pixel_, const decimal depth_) :
		pixel(pixel_), depth(depth_) {}
	};

	struct Fragment_shader_output_data {
		math::Pixel pixel{};
		decimal depth{};
		math::Color color{};

		Fragment_shader_output_data() = default;
		explicit Fragment_shader_output_data(const math::Pixel &pixel_, const decimal &depth_,const math::Color &color_) :
		pixel(pixel_), depth(depth_), color(color_)
		{}
	};

	class Shader {
	public:
		virtual ~Shader() = default;
		virtual Vertex_shader_output_data vertex_shader(const Vertex_shader_input_data& input) = 0;
		virtual Fragment_shader_output_data fragment_shader(const Fragment_shader_input_data& input) = 0;
	};

	struct Default_vertex_shader_input_data : Vertex_shader_input_data {
		math::UV main_uv{};
		math::Point3d world_position{};
		math::Vector3d normal{};

		explicit Default_vertex_shader_input_data(
				const math::Homo3d &view_position_,
				const math::Point3d &world_position_,
				const math::Vector3d &normal_,
				const math::UV &main_uv_) :
		Vertex_shader_input_data(view_position_),
		normal(normal_),
		main_uv(main_uv_),
		world_position(world_position_)
		{}
	};

	struct Default_vertex_shader_output_data : Vertex_shader_output_data {
		math::UV main_uv{};
		math::Point3d world_position{};
		math::Vector3d world_normal{};
		explicit Default_vertex_shader_output_data(
				const decimal &inv_w_,
				const math::Homo3d &clip_position_,
				const math::UV &main_uv_,
				const math::Point3d &world_position_,
				const math::Vector3d world_normal_) :
		Vertex_shader_output_data(inv_w_, clip_position_),
		main_uv(main_uv_),
		world_position(world_position_),
		world_normal(world_normal_) {}
	};

	struct Default_fragment_shader_input_data : Fragment_shader_input_data {
		math::UV main_uv{};
		math::Point3d world_position{};
		math::Vector3d world_normal{};
		explicit Default_fragment_shader_input_data(
				const math::Pixel  &pixel_,
				const decimal &depth_,
				const math::UV &main_uv_,
				const math::Point3d &world_position_,
				const math::Vector3d &world_normal_) :
		Fragment_shader_input_data(pixel_, depth_),
		main_uv(main_uv_),
		world_position(world_position_),
		world_normal(world_normal_) {}
	};

	struct Default_fragment_shader_output_data : Fragment_shader_output_data {
		explicit Default_fragment_shader_output_data(const math::Pixel &pixel_, const decimal &depth_,const math::Color &color_) :
		Fragment_shader_output_data(pixel_, depth_, color_) {}
	};

	//Simple Blinn-Phong Shader
	class Default_Shader : public Shader {
	public:
		math::Transform3d model{};
		math::Transform3d view{};
		math::Transform3d projection{};
		const Camera *camera = nullptr;
		const rendering::Lightenings *lightenings = nullptr;
		const Texture *main_texture = nullptr;

		Default_Shader() = default;
		~Default_Shader() override = default;

		Default_Shader(
				const math::Transform3d  &model_,
				const math::Transform3d  &view_,
				const math::Transform3d  &projection_,
				const gpu::Camera *camera_,
				const rendering::Lightenings *lightenings_,
				const application::Texture *main_texture_
		) :
		model(model_),
		view(view_),
		projection(projection_),
		camera(camera_),
		lightenings(lightenings_),
		main_texture(main_texture_) { }

		Default_Shader(
				math::Transform3d  &&model_,
				math::Transform3d  &&view_,
				math::Transform3d  &&projection_,
				const gpu::Camera *camera_,
				const rendering::Lightenings *lightenings_,
				const application::Texture *main_texture_
		) :
		model(std::move(model_)),
		view(std::move(view_)),
		projection(std::move(projection_)),
		camera(camera_),
		lightenings(lightenings_),
		main_texture(main_texture_) { }

		Vertex_shader_output_data vertex_shader(const Vertex_shader_input_data& input) override {

//			auto mvp = projection * view * model;
//			auto normal_mvp = mvp.inv().transpose();
//			auto normal_mvp_blocked = normal_mvp.block<double, 3, 3>(0, 0);
//			auto clip = mvp * input.position;
//			auto normal = (normal_mvp_blocked * input.normal).normalize();
//
//			return {
//					clip,
//					input.color,
//					input.uv,
//					1.0 / clip.w(),
//					normal,
//					input.world_position
//			};


		}

		Fragment_shader_output_data fragment_shader(const Fragment_shader_input_data& input) override {
			//return input;
		}
	};


}

