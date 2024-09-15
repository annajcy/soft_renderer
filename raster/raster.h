#pragma once

#include "base.h"
#include "maths.h"
#include "color.h"

class Raster {
public:
	static void rasterize_line_bresenham_colored(
		std::vector<std::pair<math::Pixel, Color>>& result,
		std::pair<math::Pixel, Color> pixel_a,
		std::pair<math::Pixel, Color> pixel_b
	) {

		if (pixel_a.first.x() > pixel_b.first.x()) std::swap(pixel_a, pixel_b);

		if (pixel_a.first == pixel_b.first) {
			result.push_back({pixel_a.first, Color::alpha_blend(pixel_a.second, pixel_b.second)});
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
			result.push_back({{x, y}, Color(red, green, blue, alpha)});
			
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

	static void rasterize_line_bresenham(
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

	static void rasterize_line_aa(
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

	static void rasterize_traingle(
		std::vector<std::pair<math::Pixel, decimal>>& result,
		const math::Point2d& a,
		const math::Point2d& b,
		const math::Point2d& c,
		int scale = 1
	) {
		math::Triangle2d triangle(a, b, c);
		auto [left_buttom, right_top] = triangle.get_AABB();
		for (int x = left_buttom.x(); x <= right_top.x(); x ++) 
			for (int y = left_buttom.y(); y <= right_top.y(); y ++) {
				std::vector<math::Point2d> sampled_points;
				math::sample_pixel(sampled_points, {x, y}, scale);
				int cnt = 0;
				for (auto &p : sampled_points) {
					if (triangle.enclose(p)) cnt ++;
				}
				result.push_back({{x, y}, (decimal)cnt / (scale * scale)});
			}
	}
};