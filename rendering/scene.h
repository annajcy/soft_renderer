#pragma once

#include "light.h"
#include "model.h"

namespace rendering{
	struct Scene{
		std::shared_ptr<Camera> camera{};
		std::shared_ptr<Lighting> lighting{};
		std::vector<std::shared_ptr<Model>> models{};

		Scene() = default;

		void set_camera(const std::shared_ptr<Camera> &camera_) {
			camera = camera_;
		}

		void set_light(const std::shared_ptr<Lighting> &lighting_) {
			lighting = lighting_;
		}

		void add_model(const std::shared_ptr<Model> &model) {
			models.push_back(model);
		}

		void add_light(const std::shared_ptr<Light> &light) {
			if (!lighting) lighting = std::make_shared<Lighting>();
			lighting->add_light(light);
		}
	};
}
