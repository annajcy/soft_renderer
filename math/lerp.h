#pragma once

#include "base.h"

namespace math {

    template<typename T, typename S>
	S interpolate_1d(std::pair<T, S> a, std::pair<T, S> b, T p) {
		auto dist_a = p - a.first;
		auto dist_b = b.first - p;
		float factor_b = (float)(dist_a) / (dist_a + dist_b);
		float factor_a = 1.0 - factor_b;
		return a.second * factor_a + b.second * factor_b;
	}

}

