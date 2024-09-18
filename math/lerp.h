#pragma once

#include "base.h"
#include "geo.h"
#include "vec.h"

namespace math {

	template<typename T>
	inline std::pair<decimal, decimal> get_factor(const T& a, const T& b, const T& p) {
		auto ap = p - a, pb = b - p, ab = b - a;
		auto alpha = (decimal)pb / ab, beta = (decimal)ap / ab;
		return {alpha, beta};
	}

	template<typename T>
	inline T calculate_weighed(const T& a, const T& b, const std::pair<decimal, decimal>& factor) {
		auto &[alpha, beta] = factor;
		return a * alpha + b * beta;
	}

    template<typename T, typename S>
    inline S interpolate(const std::pair<T, S>& a, const std::pair<T, S>& b, const T& p) {
		return calculate_weighed(a.second, b.second, get_factor(a.first, b.first, p));
	}

	inline std::tuple<decimal, decimal, decimal> get_factor(const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& p)  {
		Triangle2d abc(a, b, c), pab(p, a, b), pbc(p, b, c), pca(p, c, a);
		auto area_abc = abc.area();
		auto alpha = (decimal)pbc.area() / area_abc;
		auto beta = (decimal)pca.area() / area_abc;
		auto gamma = (decimal)pab.area() / area_abc;
		return {alpha, beta, gamma};
	}

	template<typename T>
	inline T calculate_weighed(const T& a, const T& b, const T& c, const std::tuple<decimal, decimal, decimal>& barycentric) {
		auto &[alpha, beta, gamma] = barycentric;
		return a * alpha + b * beta + c * gamma;
	}

	template<typename S>
	inline S interpolate(const std::pair<Point2d, S>& a,const std::pair<Point2d, S>& b, const std::pair<Point2d, S>& c, const Point2d& p) {
		return calculate_weighed(a.second, b.second, c.second, get_factor(a.first, b.first, c.first, p));
	}

}

