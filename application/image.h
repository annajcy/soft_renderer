#pragma once

#include "base.h"
#include "color.h"

class Image {
private:
    std::unique_ptr<math::Color[]> data;

public:
    int width, height;
    Image(int width_, int height_) : width(width_), height(height_), data(std::make_unique<math::Color[]>(width_ * height_)) { }

    Image(const std::string& path) {
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

    Image(Image&& image) : width(image.width), height(image.height), data(std::move(image.data)) { }

	int size() const { return width * height; }

	math::Color& at(int x, int y) {
        if (x < 0 || x >= width) throw std::out_of_range("Error: Out of bound of image");
        if (y < 0 || y >= height) throw std::out_of_range("Error: Out of bound of image");
        return data[y * width + x];
    }

	math::Color at(int x, int y) const {
        if (x < 0 || x >= width) throw std::out_of_range("Error: Out of bound of image");
        if (y < 0 || y >= height) throw std::out_of_range("Error: Out of bound of image");
        return data[y * width + x];
    }

	math::Color at_uv(decimal u, decimal v, bool bilinear = true) const {
		if (bilinear) return at_uv_bilinear(u, v);
		int x = std::round(u * (width - 1)), y = std::round(v * (height - 1));
		if (x < 0 || x >= width) throw std::out_of_range("Error: Out of bound of image");
		if (y < 0 || y >= height) throw std::out_of_range("Error: Out of bound of image");
		return data[y * width + x];
	}

	math::Color at_uv_bilinear(decimal u, decimal v) const {
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

};