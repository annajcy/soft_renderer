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
		std::vector<std::shared_ptr<Light>> lights{};
		explicit Lighting(const std::vector<std::shared_ptr<Light>> &lights_) : lights(lights_) {}
		explicit Lighting(std::vector<std::shared_ptr<Light>> &&lights_) : lights(std::move(lights_)) {}

		template<typename T>
		std::vector<std::shared_ptr<T>> get_lights() requires Inherited<Light, T> {
			std::vector<std::shared_ptr<T>> result{};
			for (auto &l : lights) {
				auto light_ptr = l->get_ptr<T>();
				if (light_ptr != nullptr)
					result.push_back(light_ptr);
			}
			return result;
		}
	};
}