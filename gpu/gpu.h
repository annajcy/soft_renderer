#pragma once

#include "base.h"
#include "color.h"
#include "frame_buffer.h"
#include "buffer_object.h"
#include "shader.h"

#define gpu GPU::get_instance()

enum OBJECT {
	VERTEX_ARRAY,
	VERTEX_BUFFER,
	ELEMENT_BUFFER
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

public:

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

	void set_buffer(OBJECT object, void* data, int size) {
		if (object == OBJECT::VERTEX_BUFFER) {
			if (!vbo_map.count(vbo_id)) throw std::invalid_argument("invalid id");
			vbo_map[vbo_id].set_buffer_data((decimal*)data, size);
		} else if (object == OBJECT::ELEMENT_BUFFER) {
			if (!ebo_map.count(ebo_id)) throw std::invalid_argument("invalid id");
			ebo_map[ebo_id].set_buffer_data((int*)data, size);
		}
	}

	void set_vertex_array(const VAO& vao) {
		if (!vao_map.count(vao_id)) throw std::invalid_argument("invalid id");
		vao_map[vao_id] = vao;
	}

};