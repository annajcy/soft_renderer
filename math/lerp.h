#pragma once

#include "base.h"

namespace math {

    template<typename T, typename S>
	S interpolate_1d(std::pair<T, S> a, std::pair<T, S> b, T p) {
		auto dist_ap = p - a.first;
		auto dist_pb = b.first - p;
		float factor_ap = (dist_ap) / (dist_ap + dist_pb);
		float factor_pb = 1 - factor_ap;
		return a.second * factor_ap + b.second * factor_pb;
	}

}

