#pragma once

#include "base.h"
#include "color.h"

namespace application {

	enum WRAP_MODE {
		NONE,
		REPEAT,
		MIRROR,
	};

	enum FILL_MODE {
		FLATTEN,
		FIT_WIDTH,
		FIT_HEIGHT,
	};

	class Image {
	private:
		std::unique_ptr<math::Color[]> data;

	public:
		int width, height;
		Image(int width_, int height_) : width(width_), height(height_), data(std::make_unique<math::Color[]>(width_ * height_)) { }
		Image(int width_, int height_, const math::Color &color) : width(width_), height(height_), data(std::make_unique<math::Color[]>(width_ * height_)) {
			for (int y = 0; y < height; y ++)
				for (int x = 0; x < width; x ++) {
					this->at(x, y) = color;
				}
		}
		explicit Image(const std::string& path) {
			cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED);

			if (image.empty()) throw std::runtime_error("Error: Unable to load image");

			width = image.cols, height = image.rows;
			data = std::make_unique<math::Color[]>(width * height);

			auto channels_count = image.channels();

			for (int y = 0; y < height; y ++)
				for (int x = 0; x < width; x ++) {
					math::Color color;
					if (channels_count == 4) {
						auto& pixel = image.at<cv::Vec4b>(height - 1 - y, x);
						color = math::Color(pixel[2], pixel[1], pixel[0], pixel[3]);
					} else if (channels_count == 3) {
						auto& pixel = image.at<cv::Vec3b>(height - 1 - y, x);
						color = math::Color(pixel[2], pixel[1], pixel[0]);
					} else if (channels_count == 1) {
						auto& pixel = image.at<uchar>(height - 1 - y, x);
						color = math::Color(pixel);
					} else {
						throw std::runtime_error("Error: Unknown channels count");
					}
					this->at(x, y) = color;
				}
		}
		Image(const Image& image) : width(image.width), height(image.height), data(std::make_unique<math::Color[]>(image.width * image.height)) {
			for (int y = 0; y < height; y ++)
				for (int x = 0; x < width; x ++) {
					this->at(x, y) = image.at(x, y);
				}
		}
		Image(Image&& image) noexcept : width(image.width), height(image.height), data(std::move(image.data)) { }

		[[nodiscard]] int size() const { return width * height; }
		[[nodiscard]] decimal ratio() const { return (decimal)width / height; }

		math::Color& at(int x, int y) {
			if (x < 0 || x >= width) throw std::out_of_range("Error: Out of bound of image");
			if (y < 0 || y >= height) throw std::out_of_range("Error: Out of bound of image");
			return data[y * width + x];
		}

		[[nodiscard]] math::Color at(int x, int y) const {
			if (x < 0 || x >= width) throw std::out_of_range("Error: Out of bound of image");
			if (y < 0 || y >= height) throw std::out_of_range("Error: Out of bound of image");
			return data[y * width + x];
		}

		[[nodiscard]] math::Color at_uv(decimal u, decimal v, bool bilinear = true, WRAP_MODE wrap_mode = WRAP_MODE::REPEAT) const {
			if (bilinear) return at_uv_bilinear(u, v, wrap_mode);

			if (wrap_mode == WRAP_MODE::REPEAT) {
				if (cmp(u, 0.0) == -1 || cmp(u, 1.0) == 1) u = fraction(u), u = fraction(1 + u);
				if (cmp(v, 0.0) == -1 || cmp(v, 1.0) == 1) v = fraction(v), v = fraction(1 + v);

			} else if (wrap_mode == WRAP_MODE::MIRROR) {
				if (cmp(u, 0.0) == -1 || cmp(u, 1.0) == 1) u = fraction(u), u = 1 - fraction(1 + u);
				if (cmp(v, 0.0) == -1 || cmp(v, 1.0) == 1) v = fraction(v), v = 1 - fraction(1 + v);
			} else {
				if (cmp(u, 0.0) == -1 || cmp(u, 1.0) == 1) throw std::out_of_range("Error: Out of bound of image");
				if (cmp(v, 0.0) == -1 || cmp(v, 1.0) == 1) throw std::out_of_range("Error: Out of bound of image");
			}

			int x = std::round(u * (width - 1)), y = std::round(v * (height - 1));
			return data[y * width + x];
		}

		[[nodiscard]] math::Color at_uv_bilinear(decimal u, decimal v, WRAP_MODE wrap_mode = WRAP_MODE::REPEAT) const {

			if (wrap_mode == WRAP_MODE::REPEAT) {
				if (cmp(u, 0.0) == -1 || cmp(u, 1.0) == 1) u = fraction(u), u = fraction(1 + u);
				if (cmp(v, 0.0) == -1 || cmp(v, 1.0) == 1) v = fraction(v), v = fraction(1 + v);

			} else if (wrap_mode == WRAP_MODE::MIRROR) {
				if (cmp(u, 0.0) == -1 || cmp(u, 1.0) == 1) u = fraction(u), u = 1 - fraction(1 + u);
				if (cmp(v, 0.0) == -1 || cmp(v, 1.0) == 1) v = fraction(v), v = 1 - fraction(1 + v);
			} else {
				if (cmp(u, 0.0) == -1 || cmp(u, 1.0) == 1) throw std::out_of_range("Error: Out of bound of image");
				if (cmp(v, 0.0) == -1 || cmp(v, 1.0) == 1) throw std::out_of_range("Error: Out of bound of image");
			}

			u *= (width - 1), v *= (height - 1);
			int x = std::floor(u), y = std::floor(v);

			if (x == width - 1 && y == height - 1) return this->at(x, y);

			if (x == width - 1) {
				auto factor = math::get_factor((decimal)y, (decimal)y + 1, v);
				return math::Color::interpolate_color(this->at(x, y), this->at(x, y + 1), factor);
			}

			if (y == height - 1) {
				auto factor = math::get_factor((decimal)x, (decimal)x + 1, u);
				return math::Color::interpolate_color(this->at(x, y), this->at(x + 1, y), factor);
			}

			auto factor_y = math::get_factor((decimal)y, (decimal)y + 1, v);
			auto factor_x = math::get_factor((decimal)x, (decimal)x + 1, u);

			math::Color color_left = math::Color::interpolate_color(this->at(x, y), this->at(x, y + 1), factor_y);
			math::Color color_right = math::Color::interpolate_color(this->at(x + 1, y), this->at(x + 1, y + 1), factor_y);
			math::Color color = math::Color::interpolate_color(color_left, color_right, factor_x);

			return color;
		}

		//raster image
		static void render_image_fixed(std::vector<std::pair<math::Pixel, math::Color>>& result, const Image& image, const math::Pixel& start_point = {0, 0}) {
			result.clear();
			int start_x = start_point.x(), start_y = start_point.y();
			for (int i = 0, y = start_y; i < image.height; i ++, y ++)
				for (int j = 0, x = start_x; j < image.width; j ++, x ++) {
					result.push_back({{x, y}, image.at(x, y)});
				}
		}

		static void render_image(
				std::vector<std::pair<math::Pixel, math::Color>>& result,
				const Image& image,
				const math::Pixel& start_point = {0, 0},
				int width = 100, int height = 100,
				bool bilinear = true,
				WRAP_MODE warp_mode = WRAP_MODE::REPEAT,
				FILL_MODE fill_mode = FILL_MODE::FIT_HEIGHT
		) {
			result.clear();
			int stride_x = image.width / width, stride_y = image.height / height;
			int start_x = start_point.x(), start_y = start_point.y();
			for (int i = 0, y = start_y; i < height; i ++, y ++)
				for (int j = 0, x = start_x; j < width; j ++, x ++) {

					decimal u, v;
					if (warp_mode == WRAP_MODE::NONE) {
						u = (decimal)j / width, v = (decimal)i / height;
					} else {
						if (fill_mode == FILL_MODE::FIT_HEIGHT) {
							u = (decimal)j / width * width / height / image.ratio(), v = (decimal)i / height;
						} else if (fill_mode == FILL_MODE::FIT_WIDTH) {
							u = (decimal)j / width, v = (decimal)i / height * height / width * image.ratio();
						} else {
							u = (decimal)j / image.width, v = (decimal)i / image.height;
						}
					}

					result.push_back({{x, y}, image.at_uv(u, v, bilinear, warp_mode)});

				}
		}

	};

	using Texture = Image;

};

