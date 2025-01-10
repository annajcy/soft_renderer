#pragma once

namespace math {

	inline std::vector<Point2d> sample_pixel(const Pixel& pixel, int scale) {
		std::vector<Point2d> result{};
		decimal stride = 1.0 / scale;
		decimal startup = stride / 2;

		decimal x = startup + pixel.x();
		for (int i = 0; i < scale; x += stride, i ++) {
			decimal y = startup + pixel.y();
			for (int j = 0; j < scale; y += stride, j ++) {
				result.push_back({x, y});
			}
		}

		return result;
	}

}
