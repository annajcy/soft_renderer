#pragma once

namespace gpu {

	struct GPU {

		std::shared_ptr<Color_buffer> color_buffer{};

		virtual void init(int width, int height) {
			color_buffer = std::make_shared<Color_buffer>();
			color_buffer->init(width, height, math::BGR::zero());
		}

		[[nodiscard]] int height() const { return color_buffer->height; }
		[[nodiscard]] int width() const { return color_buffer->width; }

		virtual void clear() { color_buffer->clear(); }

		[[nodiscard]] u_int8_t* color_buffer_raw() {
			return reinterpret_cast<u_int8_t*>(color_buffer->data.get());
		}

		//the color format of opencv is BGR, and y is inverted.
		void set_pixel(int x, int y, const math::Color& color, bool blend = true) {
			y = height() - y;
			if (!color_buffer->is_valid(x, y)) return;
			auto& pixel = color_buffer->at(x, y);
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
}
