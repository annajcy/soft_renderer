#pragma once

#include "base.h"
#include "color.h"
#include "frame_buffer.h"
#include "shader.h"

#define gpu GPU::get_instance()

class GPU
{
private:
    static GPU* instance;
	std::unique_ptr<Frame_buffer> frame_buffer;

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


    void clear() {
		frame_buffer->clear();
    }

	std::shared_ptr<u_int8_t[]> color_buffer() {
		return {reinterpret_cast<u_int8_t*>(frame_buffer->color_buffer.get()), [](u_int8_t*) { }};
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

};