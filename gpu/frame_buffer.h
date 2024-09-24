#pragma once

#include "base.h"

// format of BGR frame buffer
class Frame_buffer {
public:
	int width{0};
	int height{0};

	std::shared_ptr<math::BGR[]> color_buffer; // Use shared_ptr
	std::shared_ptr<decimal[]> depth_buffer;   // Use shared_ptr

	Frame_buffer() = default;

	Frame_buffer(int width_, int height_) : width(width_), height(height_) {
		color_buffer = std::shared_ptr<math::BGR[]>(new math::BGR[width_ * height_]);
		depth_buffer = std::shared_ptr<decimal[]>(new decimal[width_ * height_]);
		std::fill_n(depth_buffer.get(), width_ * height_, 1.0);
	}

	void clear() {
		std::fill_n(depth_buffer.get(), width * height, 1.0);
		memset(color_buffer.get(), 0, width * height * sizeof(math::BGR));
	}

	decimal& depth_at(int x, int y) {
		if (x < 0 || x >= width) throw std::out_of_range("out of range");
		if (y < 0 || y >= height) throw std::out_of_range("out of range");
		return depth_buffer.get()[y * width + x];
	}

	decimal depth_at(int x, int y) const {
		if (x < 0 || x >= width) throw std::out_of_range("out of range");
		if (y < 0 || y >= height) throw std::out_of_range("out of range");
		return depth_buffer.get()[y * width + x];
	}

	math::BGR& color_at(int x, int y) {
		if (x < 0 || x >= width) throw std::out_of_range("out of range");
		if (y < 0 || y >= height) throw std::out_of_range("out of range");
		return color_buffer.get()[(height - 1 - y) * width + x];
	}

	math::BGR color_at(int x, int y) const {
		if (x < 0 || x >= width) throw std::out_of_range("out of range");
		if (y < 0 || y >= height) throw std::out_of_range("out of range");
		return color_buffer.get()[(height - 1 - y) * width + x];
	}

};


