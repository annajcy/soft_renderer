#pragma once

#include "base.h"
#include "color.h"
#include "frame_buffer.h"
#include "buffer_object.h"
#include "shader.h"
#include "raster.h"

#define gpu GPU::get_instance()

enum OBJECT {
	VERTEX_ARRAY,
	VERTEX_BUFFER,
	ELEMENT_BUFFER
};

enum PRIMITIVE {
	TRIANGLE,
	LINE,
};

enum CULL_TYPE {
	FRONT,
	BACK,
	DISABLE
};

class GPU
{
private:
    static GPU* instance;

	std::unique_ptr<Frame_buffer> frame_buffer{ nullptr };

	std::unordered_map<int, VAO> vao_map{};
	std::unordered_map<int, VBO> vbo_map{};
	std::unordered_map<int, EBO> ebo_map{};
	int vbo_count{ 0 }, vao_count{ 0 }, ebo_count{ 0 };
	int vbo_id{ 0 }, vao_id{ 0 }, ebo_id{ 0 };

	std::unique_ptr<Shader> shader{ nullptr };

	GPU() = default;

	void vertex_shade(std::vector<Vertex_shader_data>& output) {

		output.clear();
		
		if (!ebo_map.contains(ebo_id)) {
			throw std::invalid_argument("invalid ebo");
		}
		
		auto& ebo = ebo_map[ebo_id];
		auto indices = ebo.get_buffer_data(0, 3, 0, ebo.size_data);

		for (int i = 0; i < ebo.size_data; i ++) {
			int vertex_id = indices[i];

			//position
			auto [id0, stride0, offset0, item_size0] = vao_map[1];
			auto position = vbo_map[id0].get_buffer_data(vertex_id, stride0, offset0, item_size0);
			auto position_f = math::to_homo_point(math::Point3d{position.get(), 3});

			//color
			auto [id1, stride1, offset1, item_size1] = vao_map[2];
			auto color = vbo_map[id1].get_buffer_data(vertex_id, stride1, offset1, item_size1);
			auto color_f = math::Color_decimal(color.get(), 4);
			
			//uv
			auto [id2, stride2, offset2, item_size2] = vao_map[3];
			auto uv = vbo_map[id2].get_buffer_data(vertex_id, stride2, offset2, item_size2);
			auto uv_f = math::UV(uv.get(), 2);

			auto vs_output = shader.get()->vertex_shader({position_f, color_f, uv_f, 1.0});
			output.push_back(std::move(vs_output));
		}
	}
	
	void clip_cull(std::vector<Vertex_shader_data>& output, std::vector<Vertex_shader_data>& input) {

		auto get_intersect = [&](
				const Vertex_shader_data& u, 
				const Vertex_shader_data& v, 
				const math::Vector4d& normal
		) -> Vertex_shader_data {

			decimal dist_u = normal.dot(u.position), dist_v = normal.dot(v.position);
			auto factor = math::get_factor(dist_u, dist_v, 0.0);
			auto position = math::calculate_weighed(u.position, v.position, factor);
			auto color = math::calculate_weighed(u.color, v.color, factor);
			auto uv = math::calculate_weighed(u.uv, v.uv, factor);
			auto inv_w = 1.0 / position.w();

			return {position, color, uv, inv_w};

		};

		auto inside = [&](const math::Vector4d& normal, const math::Homo3d& point) -> bool {
			return sign(normal.dot(point)) == 1;
		};

		auto clipper = [&](
			std::vector<Vertex_shader_data>& out, 
			const Vertex_shader_data& u, 
			const Vertex_shader_data& v, 
			const math::Vector4d& normal
		) {

			if (inside(normal, u.position) && inside(normal, v.position)) {
				out.push_back(v);
			} else if (!inside(normal, u.position) && inside(normal, v.position)) {
				out.push_back(v);
				out.push_back(get_intersect(u, v, normal));
			} else if (inside(normal, u.position) && !inside(normal, v.position)) {
				out.push_back(get_intersect(u, v, normal));
			}

		};

		std::vector<math::Vector4d> normals{
			{0.0, 0.0, 0.0, -1.0},
			{-1.0, 0.0, 0.0, -1.0},
			{1.0, 0.0, 0.0, -1.0},
			{0.0, -1.0, 0.0, -1.0},
			{0.0, 1.0, 0.0, -1.0},
			{0.0, 0.0, -1.0, -1.0},
			{0.0, 0.0, 1.0, -1.0},
		};

		std::vector<Vertex_shader_data> result;

		for (int i = 0; i + 2 < input.size(); i += 3) {

			std::vector<Vertex_shader_data> primitive{
				input[i],
				input[i + 1],
				input[i + 2]
			};

			auto ab = cast_dims<2>(input[i + 1].position) - cast_dims<2>(input[i].position);
			auto bc = cast_dims<2>(input[i + 2].position) - cast_dims<2>(input[i + 1].position);


			if (cull_type == CULL_TYPE::BACK) {
				if (sign(cross(ab, bc)) == -1) continue;
			} else if (cull_type == CULL_TYPE::FRONT) {
				if (sign(cross(ab, bc)) == 1) continue;
			}
			
			result = primitive;

			for (auto &normal : normals) {
				std::vector<Vertex_shader_data> data = result;
				result.clear();
				for (int j = 0; j < data.size(); j ++) {
					auto& u = data[j];
					auto& v = data[(j + 1) % data.size()];
					clipper(result, u, v, normal);
				}
			}

			std::sort(result.begin(), result.end(), [&](const Vertex_shader_data& u, const Vertex_shader_data& v){
				if (u.position.x() == v.position.x()) return u.position.y() < u.position.y();
				return u.position.x() < v.position.x();
			});

			for (int j = 1; j + 1 < result.size(); j ++ ) {
				output.push_back(result[0]);
				output.push_back(result[j]);
				output.push_back(result[j + 1]);
			}
		}
	}

	void perspective_division(std::vector<Vertex_shader_data>& output, std::vector<Vertex_shader_data>& input) {
		output.clear();
		for (auto &data : input) {
			data.position = math::normalize_homo_point(data.position);
			data.color *= data.inv_w;
			data.uv *= data.inv_w;
		}
		output = std::move(input);
	}

	void screen_mapping(std::vector<Vertex_shader_data>& output, std::vector<Vertex_shader_data>& input) {
		output.clear();
		for (auto &data : input) {
			data.position = math::screen(width(), height()) * data.position;
		}
		output = std::move(input);
	}

	void rasterizing(std::vector<Vertex_shader_data>& output, std::vector<Vertex_shader_data>& input) {
		output.clear();
		for (int i = 0; i < input.size(); i += 3) {
			std::vector<Vertex_shader_data> result;
			Raster::triangle_shader_data(result, input[i], input[i + 1], input[i + 2]);
			for (auto &data : result) {
				output.push_back(data);
			}
		}
	}

	void fragment_shade(std::vector<Fragment_shader_data>& output, std::vector<Vertex_shader_data>& input) {
		output.clear();
		for (auto &data : input) {
			output.push_back(shader.get()->fragment_shader(data));
		}
	}

	//TODO
	void blending(std::vector<Fragment_shader_data>& output, std::vector<Fragment_shader_data>& input) {
		output.clear();
		output = std::move(input);
	}

	void draw(std::vector<Fragment_shader_data>& input) {
		for (auto &data : input) {
			auto x = data.pixel.x(), y = data.pixel.y();
			auto depth = data.depth;
			if (depth_test_enabled) {
				if (data.depth >= frame_buffer.get()->depth_at(x, y)) {
					frame_buffer.get()->depth_at(x, y) = data.depth;
					set_pixel(x, y, data.color, blend_enabled);
				}
			} else {
				set_pixel(x, y, data.color, blend_enabled);
			}
			
		}
	}

	void draw_line() {
		throw std::invalid_argument("not implemented");
	}

	void draw_triangle() {
		std::vector<Vertex_shader_data> vertex_shade_output; vertex_shade(vertex_shade_output);
		std::vector<Vertex_shader_data> clip_cull_output; clip_cull(clip_cull_output, vertex_shade_output);
		std::vector<Vertex_shader_data> perspective_division_output; perspective_division(perspective_division_output, clip_cull_output);
		std::vector<Vertex_shader_data> screen_mapping_output; screen_mapping(screen_mapping_output, perspective_division_output);
		std::vector<Vertex_shader_data> rasterizing_output; rasterizing(rasterizing_output, screen_mapping_output);
		std::vector<Fragment_shader_data> fragment_shade_output; fragment_shade(fragment_shade_output, rasterizing_output);
		std::vector<Fragment_shader_data> blending_output; blending(blending_output, fragment_shade_output);
		draw(blending_output);
	}

public:

	//vertices arranged clockwise respresent front face
	CULL_TYPE cull_type = CULL_TYPE::DISABLE;
	PRIMITIVE primitive_type = PRIMITIVE::TRIANGLE;

	bool blend_enabled = true;
	bool depth_test_enabled = true;

    

    static GPU* get_instance() {
        if (instance == nullptr) {
            instance = new GPU();
        }
        return instance;
    }

	void init(int width, int height) {
		frame_buffer = std::make_unique<Frame_buffer>(width, height);
	}

	int height() { return frame_buffer->height; }
	int width() { return frame_buffer->width; }

    void clear() { frame_buffer->clear(); }

	std::shared_ptr<u_int8_t[]> color_buffer() {
		return { reinterpret_cast<u_int8_t*>(frame_buffer->color_buffer.get()), [](u_int8_t*) { } };
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

	int generate(OBJECT object) {
		if (object == OBJECT::VERTEX_ARRAY) {
			vao_map.insert({++ vao_count, VAO{}});
			return vao_count;
		} else if (object == OBJECT::VERTEX_BUFFER) {
			vbo_map.insert({++ vbo_count, VBO{}});
			return vbo_count;
		} else if (object == OBJECT::ELEMENT_BUFFER) {
			ebo_map.insert({++ ebo_count, EBO{}});
			return ebo_count;
		}
		return 0;
	}

	void bind(OBJECT object, int id) {
		if (object == OBJECT::VERTEX_ARRAY) {
			vao_id = id;
		} else if (object == OBJECT::VERTEX_BUFFER) {
			vbo_id = id;
		} else if (object == OBJECT::ELEMENT_BUFFER) {
			ebo_id = id;
		}
	}

	template<typename T>
	void set_buffer(OBJECT object, T* data, int size) {
		if (object == OBJECT::VERTEX_BUFFER) {
			if (!vbo_map.contains(vbo_id)) throw std::invalid_argument("invalid id");
			vbo_map[vbo_id].set_buffer_data(reinterpret_cast<void*>(data), size);
		} else if (object == OBJECT::ELEMENT_BUFFER) {
			if (!ebo_map.contains(ebo_id)) throw std::invalid_argument("invalid id");
			ebo_map[ebo_id].set_buffer_data(reinterpret_cast<void*>(data), size);
		}
	}

	void set_vertex_array(const VAO& vao) {
		if (!vao_map.contains(vao_id)) throw std::invalid_argument("invalid id");
		vao_map[vao_id] = vao;
	}

	template<typename T>
	void set_shader(const T& shader_) requires Inherited<Shader,typename std::remove_reference<T>::type> {
		shader = std::make_unique<T>(shader_);
	}

	void draw_primitive(PRIMITIVE primitive) {
		if (primitive == PRIMITIVE::TRIANGLE) {
			draw_triangle();
		} else if (primitive == PRIMITIVE::LINE) {
			draw_line();
		}
	}

	void print_state() {
		print(OBJECT::VERTEX_ARRAY);
		print(OBJECT::VERTEX_BUFFER);
		print(OBJECT::ELEMENT_BUFFER);
	}

	void print(OBJECT object) {
		if (object == OBJECT::VERTEX_ARRAY) {
			for (auto &[id, vao] : vao_map) {
				std::cout << "vao_id : " << id << std::endl;
				std::cout << vao << std::endl;
			}
		} else if (object == OBJECT::VERTEX_BUFFER) {
			for (auto &[id, vbo] : vbo_map) {
				std::cout << "vbo_id : " << id << std::endl;
				std::cout << vbo << std::endl;
			}
		} else if (object == OBJECT::ELEMENT_BUFFER) {
			for (auto &[id, ebo] : ebo_map) {
				std::cout << "ebo_id : " << id << std::endl;
				std::cout << ebo << std::endl;
			}
		}
	}

};