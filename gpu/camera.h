#pragma once

#include "base.h"
#include "maths.h"
#include "event_center.h"

enum CAMERA_MODE{
	TRANSLATE,
	ROTATE
};

class Camera{

private:
	void register_events() {
		Event_center<void, std::pair<int, int>, std::pair<decimal, decimal>>::get_instance()->register_event("on_mouse_move",
				[&](std::pair<int, int> pos, std::pair<decimal, decimal> delta){
					if (this->mode == CAMERA_MODE::ROTATE) {
						this->pitch += delta.first;
						this->yaw += delta.second;
						std::cout << "111" << std::endl;
						//std::cout << this->pitch << ' ' << this->yaw << std::endl;
					}
				});

		Event_center<void, std::pair<int, int>, std::pair<decimal, decimal>>::get_instance()->register_event("on_mouse_left_button_up",
				[&](std::pair<int, int> pos, std::pair<decimal, decimal> delta){
					this->mode = CAMERA_MODE::TRANSLATE;
					std::cout << "mode changed: translate" << std::endl;
				});

		Event_center<void, std::pair<int, int>, std::pair<decimal, decimal>>::get_instance()->register_event("on_mouse_left_button_down",
				[&](std::pair<int, int> pos, std::pair<decimal, decimal> delta){
					this->mode = CAMERA_MODE::ROTATE;
					std::cout << "mode changed: rotate" << std::endl;
				});

	}

public:
	decimal fov{ 100.0 };
	decimal aspect_ratio{ 1.0 };
	decimal near{ 0.01 };
	decimal far{ 100.0 };
	decimal yaw{ 0.0 };
	decimal pitch{ 0.0 };
	math::Point3d position{};
	math::Vector3d front{};
	math::Vector3d top{};
	CAMERA_MODE mode = CAMERA_MODE::TRANSLATE;

	Camera() = default;
	Camera(decimal fov_, decimal aspect_ratio_, decimal near_, decimal far_, const math::Point3d& front_, const math::Point3d& top_, const math::Point3d& position_) :
	fov(fov_), aspect_ratio(aspect_ratio_), near(near_), far(far_), front(front_), top(top_), position(position_) {
		register_events();
	}

	math::Transform3d get_view_matrix() {
		return math::view(front, top, position);
	}

	math::Transform3d get_projection_matrix() {
		return math::projection_perspective(fov, aspect_ratio, near, far);
	}


	math::Transform3d get_view_projection_matrix() {
		return math::projection_perspective(fov, aspect_ratio, near, far) * math::view(front, top, position);
	}

};