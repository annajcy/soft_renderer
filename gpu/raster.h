#pragma once

#include "base.h"
#include "maths.h"
#include "image.h"

namespace gpu {
	using namespace application;

	class Raster {
	public:

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

			int delta_x = b.x() - a.x();
			int delta_y = b.y() - a.y();

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

		static void triangle_shader_data(
				std::vector<Fragment_shader_data>& result,
				const Vertex_shader_data& a,
				const Vertex_shader_data& b,
				const Vertex_shader_data& c,
				int MSAA_scale = 1
		) {
			result.clear();
			auto &[point_a, color_a, uv_a, inv_a, normal_a, world_position_a] = a;
			auto &[point_b, color_b, uv_b, inv_b, normal_b, world_position_b] = b;
			auto &[point_c, color_c, uv_c, inv_c, normal_c, world_position_c] = c;

			auto pa = math::Point2d{point_a};
			auto pb = math::Point2d{point_b};
			auto pc = math::Point2d{point_c};

			math::Triangle2d triangle(pa, pb, pc);

			auto [left_bottom, right_top] = triangle.bounding_box();
			for (int x = left_bottom.x(); x <= right_top.x(); x ++)
				for (int y = left_bottom.y(); y <= right_top.y(); y ++) {

					std::vector<math::Point2d> sampled_points;
					math::sample_pixel(sampled_points, {x, y}, MSAA_scale);
					int enclosed = 0;
					for (auto &p : sampled_points) {
						if (triangle.enclose(p)) enclosed ++;
					}

					if(!enclosed) continue;

					auto transparency = (decimal)enclosed / (MSAA_scale * MSAA_scale);
					auto barycentric = math::get_factor(pa, pb, pc, math::Point2d{x, y});

					auto inv = math::calculate_weighed(inv_a, inv_b, inv_c, barycentric);
					auto depth = math::calculate_weighed(point_a.z(), point_b.z(), point_c.z(), barycentric);
					auto color = math::calculate_weighed(color_a, color_b, color_c, barycentric) / inv;
					auto uv = math::calculate_weighed(uv_a, uv_b, uv_c, barycentric) / inv;
					auto normal = math::calculate_weighed(normal_a, normal_b, normal_c, barycentric) / inv;
					auto world_position = math::calculate_weighed(world_position_a, world_position_b, world_position_c, barycentric) / inv;

					color *= transparency;

					result.push_back(
							Fragment_shader_data
									{{x, y},
									 depth,
									 math::Color(color),
									 uv,
									 normal,
									 world_position});
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

}

