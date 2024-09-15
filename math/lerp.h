#pragma once

#include "base.h"
#include "geo.h"

namespace math {

	template<typename T>
	std::pair<decimal, decimal> get_factor(const T& a, const T& b, const T& p) {
		auto ap = p - a, pb = b - p, ab = b - a;
		auto alpha = (decimal)pb / ab, beta = (decimal)ap / ab;
		return {alpha, beta};
	}

    template<typename T, typename S>
	S interpolate(const std::pair<T, S>& a, const std::pair<T, S>& b, const T& p) {
		auto [alpha, beta] = get_factor(a.first, b.first, p);
		return a.second * alpha + b.second * beta;
	}

	template<typename T>
	std::tuple<decimal, decimal, decimal> get_barycentic(const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& p)  {
		Triangle2d abc(a, b, c), pab(p, a, b), pbc(p, b, c), pca(p, c, a);
		auto area_abc = abc.area();
		auto alpha = (decimal)pbc.area() / area_abc;
		auto beta = (decimal)pca.area() / area_abc;
		auto gamma = (decimal)pab.area() / area_abc;
		return {alpha, beta, gamma};
	}

	template<typename T, typename S>
	S interpolate_barycentric(const std::pair<Point2d, S>& a,const std::pair<Point2d, S>& b, const std::pair<Point2d, S>& c, const Point2d& p) {
		auto [alpha, beta, gamma] = get_barycentic(a.first, b.first, c.first, p);
		return a.second * alpha + b.second * beta + c.second * gamma;
	}

}

