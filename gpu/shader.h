#pragma once

#include "base.h"
#include "maths.h"

struct Vertex_shader_data {
	math::Homo3d position{};
	math::Color_decimal color{};
	math::UV uv{};
	//math::Vector3d normal{};
	decimal inv_w{ 1.0 };
};

struct Fragment_shader_data {
	math::Pixel pixel{};
	decimal depth{ 0.0 };
	math::Color color{};
	math::UV uv{};
};

class Shader {
public:
	virtual ~Shader() = default;
	virtual Vertex_shader_data vertex_shader(const Vertex_shader_data& input) = 0;
	virtual Fragment_shader_data fragment_shader(const Vertex_shader_data& input) = 0;
};

class Default_Shader : public Shader {
public:
	math::Transform3d model{};
	math::Transform3d view{};
	math::Transform3d projection{};

	Default_Shader() = default;
	~Default_Shader() override = default;

	Default_Shader(
		const math::Transform3d  &model_,
		const math::Transform3d  &view_,
		const math::Transform3d  &projection_
	) : model(model_), view(view_), projection(projection_) { }

	Default_Shader(
		math::Transform3d  &&model_,
		math::Transform3d  &&view_,
		math::Transform3d  &&projection_
	) : model(std::move(model_)), view(std::move(view_)), projection(std::move(projection_)) { }

	Vertex_shader_data vertex_shader(const Vertex_shader_data& input) override {
		auto clip = projection * view * model * input.position;
		return { clip,
				 input.color,
				 input.uv, 
				 1.0 / clip.w()};
	}

	Fragment_shader_data fragment_shader(const Vertex_shader_data& input) override {
		return { cast<int, 2>(input.position),
		         input.position.z(),
				 math::Color(input.color),
				 input.uv};
	}
};