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

public:

    GPU() = default;

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

	void draw(PRIMITIVE primitive) {
		if (vao_map.contains(vao_id)) {
			throw std::invalid_argument("invalid vao");
		}
		if (vbo_map.contains(vbo_id)) {
			throw std::invalid_argument("invalid vbo");
		}
		if (ebo_map.contains(ebo_id)) {
			throw std::invalid_argument("invalid ebo");
		}

		if (primitive == PRIMITIVE::TRIANGLE) {
			draw_triangle();
		}
	}

	void draw_triangle() {
		std::vector<Vertex_shader_data> vertex_shade_output; vertex_shade(vertex_shade_output);
		std::vector<Vertex_shader_data> clip_output; clip(clip_output, vertex_shade_output);
		std::vector<Vertex_shader_data> perspective_division_output; perspective_division(perspective_division_output, clip_output);
		std::vector<Vertex_shader_data> cull_output; cull(cull_output, perspective_division_output);
		std::vector<Vertex_shader_data> screen_mapping_output; screen_mapping(screen_mapping_output, cull_output);
		std::vector<Vertex_shader_data> rasterizing_output; rasterizing(rasterizing_output, screen_mapping_output);
		std::vector<Fragment_shader_data> fragment_shade_output; fragment_shade(fragment_shade_output, rasterizing_output);
		std::vector<Fragment_shader_data> blending_output; blending(blending_output, fragment_shade_output);
		draw(blending_output);
	}

	void vertex_shade(std::vector<Vertex_shader_data>& output) {
		output.clear();
		auto& ebo = ebo_map[ebo_id];
		auto indices = ebo.get_buffer_data(0, 3, 0, 3);

		for (int i = 0; i < 3; i ++) {
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
	
	//TODO
	void clip(std::vector<Vertex_shader_data>& output, std::vector<Vertex_shader_data>& input) {
		output.clear();	
		output = std::move(input);
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

	//TODO
	void cull(std::vector<Vertex_shader_data>& output, std::vector<Vertex_shader_data>& input) {
		output.clear();
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

	//TODO
	void draw(std::vector<Fragment_shader_data>& input) {
		for (auto &data : input) {
			set_pixel(data.pixel.x(), data.pixel.y(), data.color);
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