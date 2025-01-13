#pragma once

#include "base.h"
#include "color.h"
#include "frame_buffer.h"
#include "shader.h"
#include "mesh.h"
#include "gpu.h"

namespace raster {
	using namespace gpu;

	enum CULL_TYPE {
		FRONT,
		BACK,
		DISABLE
	};

	class Rasterizer : public GPU
	{

	private:
		static Rasterizer* instance;
		std::shared_ptr<Shader> shader{};
		std::shared_ptr<Depth_buffer> depth_buffer{};
		Rasterizer() = default;

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

					bool enclosed = triangle.enclose(math::Point2d {x, y});
					if (!enclosed) continue;

					auto barycentric = math::get_factor(pa, pb, pc, math::Point2d{x, y});

					Intermediate_shader_data interpolated_data = Intermediate_shader_data::interpolate_intermediate_shader_data(
							input[0], input[1], input[2], barycentric
							).perspective_recover();

					Fragment_shader_input_data frag{};

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

		bool blend_enabled = true;
		bool depth_test_enabled = true;
		bool depth_update_enabled = true;

		void init(int width, int height) override {
			GPU::init(width, height);
			depth_buffer = std::make_shared<Depth_buffer>();
			depth_buffer->init(width, height, std::numeric_limits<decimal>::infinity());
		}

		static Rasterizer* get_instance() {
			if (instance == nullptr) {
				instance = new Rasterizer();
			}
			return instance;
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
				for (auto i : {0, 1, 2}) {
					intermediate_surface[i].perspective_divide();
				}

				//screen map
				std::array<Intermediate_shader_data, 3> screen_surface{};
				for (auto i : {0, 1, 2}) {
					screen_surface[i] = screen_map(intermediate_surface[i]);
				}

				surfaces.push_back(screen_surface);
			}


			for (auto &surface : surfaces) {
				std::vector<Fragment_shader_input_data> fs_data = rasterize_triangle(surface);

				if (fs_data.empty()) continue;

				//fragment shade
				std::vector<Final_shader_data> final_fragments;
				final_fragments.reserve(fs_data.size());
				for (auto &fragment : fs_data) {
					final_fragments.push_back(shader->fragment_shader(fragment));
				}

				//set_pixel and test z buffer
				for (auto &fragment : final_fragments) {
					auto depth = fragment.depth;
					int x = fragment.pixel_position.x(), y = fragment.pixel_position.y();
					if (!depth_buffer->is_valid(x, y)) continue;
					if (depth_test_enabled) {
						if (sign(depth_buffer->at(x, y) - depth) > 0) {
							if (depth_update_enabled) depth_buffer->at(x, y) = depth;
							set_pixel(x, y, fragment.color, blend_enabled);
						}
					} else {
						set_pixel(x, y, fragment.color, blend_enabled);
					}
				}
			}
		}

		static void line_bresenham(
				std::vector<math::Pixel>& result,
				math::Pixel a,
				math::Pixel b
		)  {

			if (a.x() > b.x()) std::swap(a, b);

			if (a == b) {
				result.push_back(a);
				return;
			}

			bool down = false, surge = false;
			result.clear();

			int delta_x = b.x() - a.x();
			int delta_y = b.y() - a.y();

			if (delta_y < 0) {
				b.y() = -b.y(), a.y() = -a.y();
				delta_y = b.y() - a.y();
				down = true;
			}

			if (delta_y > delta_x) {
				std::swap(b.x(), b.y()), std::swap(a.x(), a.y());
				delta_x = b.x() - a.x();
				delta_y = b.y() - a.y();
				surge = true;
			}

			//decimal mid_y = -delta_y * (a.x() + 1) + delta_x * (a.y() + 0.5) + a.x() * b.y() - b.x() * a.y();
			// f(x) * 2 won't effect its relation between 0;
			int mid_y = 2 * -delta_y * (a.x() + 1) + delta_x * (2 * a.y() + 1) + 2 * a.x() * b.y() - 2 * b.x() * a.y();

			for (int x = a.x(), y = a.y(); x <= b.x(); x ++) {
				result.push_back({x, y});
				if (mid_y < 0) {
					y ++;
					mid_y += 2 * (delta_x - delta_y);
				} else {
					mid_y -= 2 * delta_y;
				}
			}

			if (surge) {
				for (auto &p : result)
					std::swap(p.x(), p.y());
			}

			if (down) {
				for (auto &p : result)
					p.y() = -p.y();
			}
		}
	};
}

