#pragma once

#include "maths.h"

namespace curve {

	inline void line_bresenham(
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
}
