#pragma once

#include "maths.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <initializer_list>

namespace rendering {

	struct Light : std::enable_shared_from_this<Light> {
		math::Vector3d intensity{};
		math::Vector3d color{};

		Light(const math::Color &color_, const math::Vector3d &intensity_)
				: color(color_.to_color_decimal()), intensity(intensity_) {}

		Light(math::Color&& color_, math::Vector3d&& intensity_)
				: color(color_.to_color_decimal()), intensity(std::move(intensity_)) {}

		template<typename T>
		std::shared_ptr<T> get_ptr() requires Inherited<Light, T> {
			return std::dynamic_pointer_cast<T>(shared_from_this());
		}

		virtual ~Light() = default;
	};

	struct Directional_light : public Light {
		math::Vector3d direction{};

		Directional_light(math::Color &&color_, math::Vector3d &&intensity_, math::Vector3d &&direction_)
				: Light(color_, intensity_), direction(std::move(direction_)) {}

		Directional_light(const math::Color &color_, const math::Vector3d &intensity_, const math::Vector3d &direction_)
				: Light(color_, intensity_), direction(direction_) {}

		[[nodiscard]] math::Vector3d propagate() const {
			return color * intensity;
		}
	};

	struct Point_light : public Light {
		math::Point3d origin{};

		Point_light(math::Color &&color_, math::Vector3d &&intensity_, math::Vector3d &&origin_)
				: Light(color_, intensity_), origin(std::move(origin_)) {}

		Point_light(const math::Color &color_, const math::Vector3d &intensity_, const math::Vector3d &origin_)
				: Light(color_, intensity_), origin(origin_) {}

		[[nodiscard]] math::Vector3d propagate(decimal r) const {
			return color * (intensity / (r * r));
		}
	};

	struct Ambient_light : public Light {
		Ambient_light(math::Color &&color_, math::Vector3d &&intensity_)
				: Light(color_, intensity_) {}

		Ambient_light(const math::Color &color_, const math::Vector3d &intensity_)
				: Light(color_, intensity_) {}

		[[nodiscard]] math::Vector3d propagate() const {
			return color * intensity;
		}
	};

	struct Lighting {
		std::vector<Point_light> point_lights{};
		std::vector<Directional_light> directional_lights{};
		std::vector<Ambient_light> ambient_lights{};
		Lighting() = default;
		Lighting(const std::vector<Point_light> &point_lights_,
				const std::vector<Directional_light> &directional_lights_,
				const std::vector<Ambient_light> &ambient_lights_) :
				point_lights(point_lights_),
				directional_lights(directional_lights_),
				ambient_lights(ambient_lights_) { }

		Lighting(std::vector<Point_light> &&point_lights_,
				 std::vector<Directional_light> &&directional_lights_,
				 std::vector<Ambient_light> &&ambient_lights_) :
				point_lights(std::move(point_lights_)),
				directional_lights(std::move(directional_lights_)),
				ambient_lights(std::move(ambient_lights_)) { }
	};
}