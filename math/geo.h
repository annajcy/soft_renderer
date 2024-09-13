#pragma once

#include "base.h"
#include "color.h"

namespace math {
	using Point2di = Vec2i;
	using Point2d = Vec2f;
	using Point3d = Vec3f;
	using Vector2d = Vec2f;
	using Vector3d = Vec3f;
	using Homo2d = Vec3f;
	using Homo3d = Vec4f;

	using Pixel_color = std::pair<Point2di, Color>;
	using Pixel_alpha = std::pair<Point2di, float>;

	Point2d get_point2di_center(const Point2di& point) {
		return Point2d({point.x() + 0.5f, point.y() + 0.5f});
	}

}

