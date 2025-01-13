#pragma once

#include "base.h"

namespace gpu {

	template<typename T>
	struct Buffer {
		int width{};
		int height{};
		T default_value{};
		std::shared_ptr<T[]> data{ nullptr }; // Use shared_ptr

		void init(int width_, int height_, T default_value_) {
			width = width_;
			height = height_;
			default_value = default_value_;

			// Allocate the array with std::shared_ptr using a custom deleter
			data = std::shared_ptr<T[]>(new T[width_ * height_], std::default_delete<T[]>());
			std::fill_n(data.get(), width * height, default_value);
		}

		void clear() {
			std::fill_n(data.get(), width * height, default_value);
		}

		[[nodiscard]] bool is_valid(int x, int y) const {
			if (x < 0 || x >= width) return false;
			if (y < 0 || y >= height) return false;
			return true;
		}

		[[nodiscard]] T& at(int x, int y) {
			if (!is_valid(x, y)) throw std::out_of_range("out of range");
			return data[y * width + x];
		}

		[[nodiscard]] T at(int x, int y) const {
			if (!is_valid(x, y)) throw std::out_of_range("out of range");
			return data[y * width + x];
		}

		Buffer() = default;
	};

	using Color_buffer = Buffer<math::BGR>;
	using Depth_buffer = Buffer<decimal>;

}




