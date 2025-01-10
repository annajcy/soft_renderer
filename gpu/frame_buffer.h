#pragma once

#include "base.h"

namespace gpu {
	// format of BGR frame buffer
	class Frame_buffer {
	public:
		int width{ 0 };
		int height{ 0 };

		std::shared_ptr<math::BGR[]> color_buffer{ nullptr }; // Use shared_ptr
		std::shared_ptr<decimal[]> depth_buffer{ nullptr };   // Use shared_ptr

		Frame_buffer() = default;

		Frame_buffer(int width_, int height_) : width(width_), height(height_) {
			color_buffer = std::shared_ptr<math::BGR[]>(new math::BGR[width_ * height_]);
			depth_buffer = std::shared_ptr<decimal[]>(new decimal[width_ * height_]);
			std::fill_n(depth_buffer.get(), width_ * height_, std::numeric_limits<double>::infinity());
		}

		void clear() const {
			std::fill_n(depth_buffer.get(), width * height, std::numeric_limits<double>::infinity());
			memset(color_buffer.get(), 0, width * height * sizeof(math::BGR));
		}

		[[nodiscard]] decimal& depth_at(int x, int y) {
			if (!is_valid(x, y)) throw std::out_of_range("out of range");
			return depth_buffer.get()[y * width + x];
		}

		[[nodiscard]] decimal depth_at(int x, int y) const {
			if (!is_valid(x, y)) throw std::out_of_range("out of range");
			return depth_buffer.get()[y * width + x];
		}

		[[nodiscard]] math::BGR& color_at(int x, int y) {
			if (!is_valid(x, y)) throw std::out_of_range("out of range");
			return color_buffer.get()[(height - 1 - y) * width + x];
		}

		[[nodiscard]] math::BGR color_at(int x, int y) const {
			if (!is_valid(x, y)) throw std::out_of_range("out of range");
			return color_buffer.get()[(height - 1 - y) * width + x];
		}

		[[nodiscard]] bool is_valid(int x, int y) const {
			if (x < 0 || x >= width) return false;
			if (y < 0 || y >= height) return false;
			return true;
		}

	};

}




