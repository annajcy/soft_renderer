#pragma once

#include "base.h"
#include "color.h"
#include "frame_buffer.h"
#include "buffer_object.h"
#include "shader.h"
#include "raster.h"
#include "mesh.h"

namespace gpu {

	enum CULL_TYPE {
		FRONT,
		BACK,
		DISABLE
	};

	class GPU
	{
	private:
		static GPU* instance;

		std::shared_ptr<Frame_buffer> frame_buffer{ nullptr };
		std::shared_ptr<Shader> shader{ nullptr };

		GPU() = default;

		[[nodiscard]] bool cull(const std::array<Intermediate_shader_data, 3> &face) const {
			//if triangle is faced backward, then discard

			math::Point2d a = cast_dims<2>(to_point((face[0].position)));
			math::Point2d b = cast_dims<2>(to_point((face[1].position)));
			math::Point2d c = cast_dims<2>(to_point((face[2].position)));

			math::Vector2d ab = b - a;
			math::Vector2d bc = c - b;

			if (cull_type == CULL_TYPE::BACK) {
				if (sign(cross(ab, bc)) == -1) return false;
				return true;
			} else if (cull_type == CULL_TYPE::FRONT) {
				if (sign(cross(ab, bc)) == 1) return false;
				return true;
			}

			return true;
		}

		[[nodiscard]] Intermediate_shader_data divide(const Intermediate_shader_data &input) {
			//corrected perspective division
			Intermediate_shader_data output{};

			decimal w = input.position.w();
			output.depth = input.depth / w;
			output.inv_w = input.inv_w / w;
			output.base_color = input.base_color / w;
			output.uv = input.uv / w;
			output.view_normal = input.view_normal / w;
			output.view_position = input.view_position / w;
			output.position = input.position;

			return output;
		}

		[[nodiscard]] std::vector<std::array<Intermediate_shader_data, 3>> clip(const std::array<Intermediate_shader_data, 3> &face) {
			//Use Sutherland-Hodgman to clip vertices outside the canonical cube

			std::vector<std::array<Intermediate_shader_data, 3>> output{};

			std::vector<math::Vector4d> normals{
					{0.0, 0.0, 0.0, -1.0},
					{-1.0, 0.0, 0.0, -1.0},
					{1.0, 0.0, 0.0, -1.0},
					{0.0, -1.0, 0.0, -1.0},
					{0.0, 1.0, 0.0, -1.0},
					{0.0, 0.0, -1.0, -1.0},
					{0.0, 0.0, 1.0, -1.0},
			};

			std::vector<Intermediate_shader_data> vertices{};
			for (auto i : {0, 1, 2}) vertices.push_back(face[i]);

			for (auto &normal : normals) {
				std::vector<Intermediate_shader_data> temp_vertices = vertices; vertices.clear();
				for (int i = 0; i < temp_vertices.size(); i ++) {
					auto &u = temp_vertices[i];
					auto &v = temp_vertices[(i + 1) % temp_vertices.size()];

					bool is_u_inside = sign(normal.dot(u.position));
					bool is_v_inside = sign(normal.dot(v.position));

					decimal dist_u = normal.dot(u.position);
					decimal dist_v = normal.dot(v.position);
					auto factor = math::get_factor(dist_u, dist_v, 0.0);

					Intermediate_shader_data intersect = Intermediate_shader_data::interpolate_intermediate_shader_data(u, v, factor);

					if (!is_u_inside && !is_v_inside) continue;

					if (is_v_inside) vertices.push_back(v);
					if (!is_u_inside) vertices.push_back(intersect);

				}
			}

			for (int i = 1; i + 1 < vertices.size(); i ++) {
				std::array<Intermediate_shader_data, 3> result{
						vertices[0],
						vertices[i],
						vertices[i + 1]
				};
				output.push_back(result);
			}

			return output;
		}

		[[nodiscard]] Intermediate_shader_data screen_map(const Intermediate_shader_data &input) {
			Intermediate_shader_data output = input;
			output.position = math::screen(width(), height()) * normalize_homo_point(input.position);
			return output;
		}

		[[nodiscard]] std::vector<Fragment_shader_input_data> rasterize_triangle(const std::array<Intermediate_shader_data, 3> &input) const {

			std::vector<Fragment_shader_input_data> output{};

			auto pa = cast_dims<2>(input[0].position);
			auto pb = cast_dims<2>(input[1].position);
			auto pc = cast_dims<2>(input[2].position);

			math::Triangle2d triangle(pa, pb, pc);
			auto [left_bottom, right_top] = triangle.bounding_box();
			for (int x = left_bottom.x(); x <= right_top.x(); x ++)
				for (int y = left_bottom.y(); y <= right_top.y(); y ++) {

					std::vector<math::Point2d> sampled_points =
							math::sample_pixel(math::Pixel{x, y}, MSAA);

					int enclosed = 0;
					for (auto &p : sampled_points)
						if (triangle.enclose(p)) enclosed ++;
					if(!enclosed) continue;

					auto barycentric = math::get_factor(pa, pb, pc, math::Point2d{x, y});

					Intermediate_shader_data interpolated_data = Intermediate_shader_data::interpolate_intermediate_shader_data(
							input[0], input[1], input[2], barycentric
							).perspective_recover();

					Fragment_shader_input_data frag{};

					frag.transparency = (decimal)enclosed / (MSAA * MSAA);
					frag.depth = interpolated_data.depth;
					frag.base_color = interpolated_data.base_color;
					frag.uv = interpolated_data.uv;
					frag.view_position = interpolated_data.view_position;
					frag.view_normal = interpolated_data.view_normal.normalize();
					frag.pixel_position = math::Pixel{x, y};

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

					output.push_back(frag);
				}

			return output;
		}


	public:

		//vertices arranged clockwise represent front face
		CULL_TYPE cull_type = CULL_TYPE::DISABLE;

		int MSAA = 1;
		bool blend_enabled = false;
		bool depth_test_enabled = true;
		bool depth_update_enabled = true;

		static GPU* get_instance() {
			if (instance == nullptr) {
				instance = new GPU();
			}
			return instance;
		}

		void init(int width, int height) { frame_buffer = std::make_shared<Frame_buffer>(width, height); }
		int height() { return frame_buffer->height; }
		int width() { return frame_buffer->width; }
		void clear() { frame_buffer->clear(); }

		u_int8_t* color_buffer_raw() {
			return reinterpret_cast<u_int8_t*>(frame_buffer->color_buffer.get());
		}

		//the color format of opencv is BGR
		void set_pixel(int x, int y, const math::Color& color, bool blend = true) {
			if (x < 0 || x >= width()) return;
			if (y < 0 || y >= height()) return;
			auto& pixel = frame_buffer->color_at(x, y);
			if (blend) {
				math::Color background_color(pixel.r, pixel.g, pixel.b);
				auto [r, g, b, _] = math::Color::alpha_blend(color, background_color);
				pixel = {b, g, r};
			} else {
				auto [r, g, b, _] = color;
				pixel = {b, g, r};
			}
		}

		template<typename T>
		void set_shader(const std::shared_ptr<T> shader_) requires Inherited<Shader, typename std::remove_reference<T>::type> {
			shader = shader_;
		}

		void draw_model(const std::shared_ptr<mesh::Model> &model) {

			std::vector<std::array<Intermediate_shader_data, 3>> surfaces;

			for (int fid = 0; fid < model->face_count(); fid ++) {

				//prepare data
				std::array<Vertex_shader_input_data, 3> face_vs_data{};
				for (int vid : {0, 1, 2}) {
					Vertex_shader_input_data input{};
					input.uv = model->uv(fid, vid);
					input.position = model->position(fid, vid);
					input.normal = model->normal(fid, vid);
					input.base_color = math::Color::white().to_color_decimal();
					face_vs_data[vid] = input;
				}

				//vertex shade
				std::array<Intermediate_shader_data, 3> intermediate_surface{};
				for (int vid : {0, 1, 2}) {
					intermediate_surface[vid] = shader->vertex_shader(face_vs_data[vid]);
				}

				//cull
				if (!cull(intermediate_surface)) continue;

				//perspective_divide
				std::array<Intermediate_shader_data, 3> perspective_divided_surface{};
				for (auto i : {0, 1, 2}) {
					perspective_divided_surface[i] = divide(intermediate_surface[i]);
				}

				//clip
				auto clipped_surface = clip(perspective_divided_surface);
				if (clipped_surface.empty()) continue;

				//screen_mapping
				std::vector<std::array<Intermediate_shader_data, 3>> screen_surface{};
				for (auto &surface : clipped_surface)  {
					std::array<Intermediate_shader_data, 3> face{};
					for (auto i : {0, 1, 2}) face[i] = screen_map(surface[i]);
					screen_surface.push_back(face);
				}

				//save
				for (auto &surface : screen_surface)
					surfaces.push_back(surface);
			}


			for (auto &surface : surfaces) {
				std::vector<Fragment_shader_input_data> fs_data = rasterize_triangle(surface);

				if (fs_data.empty()) continue;

				//fragment Shade
				std::vector<Final_shader_data> final_fragments;
				final_fragments.reserve(fs_data.size());
				for (auto &fragment : fs_data) {
					final_fragments.push_back(shader->fragment_shader(fragment));
				}

				//set_pixel and test z buffer
				for (auto &fragment : final_fragments) {
					auto depth = fragment.depth;
					int x = fragment.pixel_position.x(), y = fragment.pixel_position.y();
					if (depth_test_enabled) {
						if (sign(frame_buffer->depth_at(x, y) - depth) > 0) {
							if (depth_update_enabled) frame_buffer->depth_at(x, y) = depth;
							set_pixel(x, y, fragment.color, blend_enabled);
						}
					} else {
						set_pixel(x, y, fragment.color, blend_enabled);
					}
				}

			}

		}

	};

}

