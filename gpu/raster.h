#pragma once

#include "base.h"
#include "maths.h"
#include "color.h"
#include "image.h"

class Raster {
public:
	static void line_bresenham_colored(
		std::vector<std::pair<math::Pixel, math::Color>>& result,
		std::pair<math::Pixel, math::Color> pixel_a,
		std::pair<math::Pixel, math::Color> pixel_b
	) {

		if (pixel_a.first.x() > pixel_b.first.x()) std::swap(pixel_a, pixel_b);

		if (pixel_a.first == pixel_b.first) {
			result.push_back({pixel_a.first, math::Color::alpha_blend(pixel_a.second, pixel_b.second)});
			return;
		}

		auto &[a, color_a] = pixel_a;
		auto &[b, color_b] = pixel_b;

		bool down = false, surge = false;
		result.clear();

		auto delta_x = b.x() - a.x();
		auto delta_y = b.y() - a.y();

		if (delta_y < 0) {
			b.y() = -b.y(), a.y() = -a.y();
			delta_y = b.y() - a.y();
			down = true;
		}

		if (delta_y > delta_x) {
			std::swap(b.x(), b.y()), std::swap(a.x(), a.y());
			delta_x = b.x() - a.x();
			delta_y = b.y() - a.y();
			surge = true;
		}

		//decimal mid_y = -delta_y * (a.x() + 1) + delta_x * (a.y() + 0.5) + a.x() * b.y() - b.x() * a.y();
		// f(x) * 2 won't effect its relation between 0;
		int mid_y = 2 * -delta_y * (a.x() + 1) + delta_x * (2 * a.y() + 1) + 2 * a.x() * b.y() - 2 * b.x() * a.y();

		for (int x = a.x(), y = a.y(); x <= b.x(); x ++) {

			auto red = math::interpolate<int, int>({a.x(), color_a.R()}, {b.x(), color_b.R()}, x);
			auto green = math::interpolate<int, int>({a.x(), color_a.G()}, {b.x(), color_b.G()}, x);
			auto blue = math::interpolate<int, int>({a.x(), color_a.B()}, {b.x(), color_b.B()}, x);
			auto alpha = math::interpolate<int, int>({a.x(), color_a.A()}, {b.x(), color_b.A()}, x);
			result.push_back({{x, y}, math::Color(red, green, blue, alpha)});
			
			if (mid_y < 0) {
				y ++;
				mid_y += 2 * (delta_x - delta_y);
			} else {
				mid_y -= 2 * delta_y;
			}
		}

		if (surge) {
			for (auto &p : result)
				std::swap(p.first.x(), p.first.y());
		}

		if (down) {
			for (auto &p : result)
				p.first.y() = -p.first.y();
		}

	}

	static void line_bresenham(
		std::vector<math::Pixel>& result,
		math::Pixel a,
		math::Pixel b
	)  {

		if (a.x() > b.x()) std::swap(a, b);

		if (a == b) {
			result.push_back(a);
			return;
		}

		bool down = false, surge = false;
		result.clear();
		
		auto delta_x = b.x() - a.x();
		auto delta_y = b.y() - a.y();

		if (delta_y < 0) {
			b.y() = -b.y(), a.y() = -a.y();
			delta_y = b.y() - a.y();
			down = true;
		}

		if (delta_y > delta_x) {
			std::swap(b.x(), b.y()), std::swap(a.x(), a.y());
			delta_x = b.x() - a.x();
			delta_y = b.y() - a.y();
			surge = true;
		}

		//decimal mid_y = -delta_y * (a.x() + 1) + delta_x * (a.y() + 0.5) + a.x() * b.y() - b.x() * a.y();
		// f(x) * 2 won't effect its relation between 0;
		int mid_y = 2 * -delta_y * (a.x() + 1) + delta_x * (2 * a.y() + 1) + 2 * a.x() * b.y() - 2 * b.x() * a.y();

		for (int x = a.x(), y = a.y(); x <= b.x(); x ++) {
			result.push_back({x, y});
			if (mid_y < 0) {
				y ++;
				mid_y += 2 * (delta_x - delta_y);
			} else {
				mid_y -= 2 * delta_y;
			}
		}

		if (surge) {
			for (auto &p : result)
				std::swap(p.x(), p.y());
		}

		if (down) {
			for (auto &p : result)
				p.y() = -p.y();
		}
	}

	static void line_alpha(
		std::vector<std::pair<math::Pixel, decimal>>& result,
		math::Pixel a,
		math::Pixel b
	)  {
		
		if (a.x() > b.x()) std::swap(a, b);

		if (a == b) {
			result.push_back({a, 1.0f});
			return;
		}

		bool down = false, surge = false;
		result.clear();

		auto delta_x = b.x() - a.x();
		auto delta_y = b.y() - a.y();

		if (delta_y < 0) {
			b.y() = -b.y(), a.y() = -a.y();
			delta_y = b.y() - a.y();
			down = true;
		}

		if (delta_y > delta_x) {
			std::swap(b.x(), b.y()), std::swap(a.x(), a.y());
			delta_x = b.x() - a.x();
			delta_y = b.y() - a.y();
			surge = true;
		}

		for (int x = a.x(); x <= b.x(); x ++) {
			decimal y = ((decimal)delta_y / delta_x) * (x - a.x()) + a.y();
			auto alpha0 = std::ceil(y) - y;
			auto alpha1 = y - std::ceil(y - 1);
			result.push_back({{x, (int)std::floor(y)}, alpha0});
			result.push_back({{x, (int)std::ceil(y)}, alpha1});
		}

		if (surge) {
			for (auto &p : result)
				std::swap(p.first.x(), p.first.y());
		}

		if (down) {
			for (auto &p : result)
				p.first.y() = -p.first.y();
		}
	}

	static void triangle_alpha(
		std::vector<std::pair<math::Pixel, decimal>>& result,
		const math::Point2d& a,
		const math::Point2d& b,
		const math::Point2d& c,
		int scale = 1
	) {
		result.clear();
		math::Triangle2d triangle(a, b, c);
		auto [left_buttom, right_top] = triangle.get_AABB();
		for (int x = left_buttom.x(); x <= right_top.x(); x ++) 
			for (int y = left_buttom.y(); y <= right_top.y(); y ++) {
				std::vector<math::Point2d> sampled_points;
				math::sample_pixel(sampled_points, {x, y}, scale);
				int enclosed = 0;
				for (auto &p : sampled_points) {
					if (triangle.enclose(p)) enclosed ++;
				}
				auto factor = (decimal)enclosed / sampled_points.size();
				result.push_back({{x, y}, factor});
			}
	}

	static void triangle_colored(
		std::vector<std::pair<math::Pixel, math::Color>>& result,
		const std::pair<math::Point2d, math::Color>& a,
		const std::pair<math::Point2d, math::Color>& b,
		const std::pair<math::Point2d, math::Color>& c,
		int scale = 1
	) {
		result.clear();
		auto &[point_a, color_a] = a;
		auto &[point_b, color_b] = b;
		auto &[point_c, color_c] = c;
		math::Triangle2d triangle(point_a, point_b, point_c);
		auto [left_buttom, right_top] = triangle.get_AABB();
		for (int x = left_buttom.x(); x <= right_top.x(); x ++) 
			for (int y = left_buttom.y(); y <= right_top.y(); y ++) {
				
				std::vector<math::Point2d> sampled_points;
				math::sample_pixel(sampled_points, {x, y}, scale);
				int enclosed = 0;
				for (auto &p : sampled_points) {
					if (triangle.enclose(p)) enclosed ++;
				}

				auto factor = (decimal)enclosed / (scale * scale);
				auto barycentric = math::get_factor(point_a, point_b, point_c, {x, y});

				auto color = math::Color::interpolate_color(color_a, color_b, color_c, barycentric);
				color *= factor;

				result.push_back({{x, y}, color});
			}
	}

	static void triangle_textured(
		std::vector<std::pair<math::Pixel, math::Color>>& result,
		const std::pair<math::Point2d, math::UV>& a,
		const std::pair<math::Point2d, math::UV>& b,
		const std::pair<math::Point2d, math::UV>& c,
		const Image& image,
		int scale = 1,
		bool bilinear = true,
		WRAP_MODE warp_mode = WRAP_MODE::REPEAT
	) {
		result.clear();
		auto &[point_a, uv_a] = a;
		auto &[point_b, uv_b] = b;
		auto &[point_c, uv_c] = c;
		math::Triangle2d triangle(point_a, point_b, point_c);
		auto [left_bottom, right_top] = triangle.get_AABB();
		for (int x = left_bottom.x(); x <= right_top.x(); x ++)
			for (int y = left_bottom.y(); y <= right_top.y(); y ++) {
				
				std::vector<math::Point2d> sampled_points;
				math::sample_pixel(sampled_points, {x, y}, scale);
				int enclosed = 0;
				for (auto &p : sampled_points) {
					if (triangle.enclose(p)) enclosed ++;
				}

				auto factor = (decimal)enclosed / (scale * scale);
				auto barycentric = math::get_factor(point_a, point_b, point_c, {x, y});
				auto uv_x = math::calculate_weighed(uv_a.x(), uv_b.x(), uv_c.x(), barycentric);
				auto uv_y = math::calculate_weighed(uv_a.y(), uv_b.y(), uv_c.y(), barycentric);

				clamp(uv_x, 0.0, 1.0), clamp(uv_y, 0.0, 1.0);

				result.push_back({{x, y}, image.at_uv(uv_x, uv_y, bilinear, warp_mode) * factor});
			}
	}

	static void image_fixed(std::vector<std::pair<math::Pixel, math::Color>>& result, const Image& image, const math::Pixel& start_point = {0, 0}) {
		result.clear();
		int start_x = start_point.x(), start_y = start_point.y();
		for (int i = 0, y = start_y; i < image.height; i ++, y ++)
			for (int j = 0, x = start_x; j < image.width; j ++, x ++) {
				result.push_back({{x, y}, image.at(x, y)});
			}
	}

	static void image(
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
					} else if (fill_mode == FILL_MODE::FIT_WITDTH) {
						u = (decimal)j / width, v = (decimal)i / height * height / width * image.ratio(); 
					} else {
						u = (decimal)j / image.width, v = (decimal)i / image.height; 
					}
				}

				//std::cout << u << ' ' << v << std::endl;
				result.push_back({{x, y}, image.at_uv(u, v, bilinear, warp_mode)});

			}
	}

};