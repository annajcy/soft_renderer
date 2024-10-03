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

		Event_center<void>::get_instance()->register_event("key_w_down", 
		[&](){
			if (this->mode == CAMERA_MODE::TRANSLATE) {
				std::cout << "translated up" << std::endl;
				translate(up(), translate_sensitivity);
			}
		});

		Event_center<void>::get_instance()->register_event("key_s_down", 
		[&](){
			if (this->mode == CAMERA_MODE::TRANSLATE) {
				std::cout << "translated down" << std::endl;
				translate(down(), translate_sensitivity);
			}
		});

		Event_center<void>::get_instance()->register_event("key_a_down", 
		[&](){
			if (this->mode == CAMERA_MODE::TRANSLATE) {
				std::cout << "translated left" << std::endl;
				translate(left(), translate_sensitivity);
			}
		});

		Event_center<void>::get_instance()->register_event("key_d_down", 
		[&](){
			if (this->mode == CAMERA_MODE::TRANSLATE) {
				std::cout << "translated right" << std::endl;
				translate(right(), translate_sensitivity);
			}
		});

		Event_center<void>::get_instance()->register_event("key_z_down", 
		[&](){
			if (this->mode == CAMERA_MODE::TRANSLATE) {
				std::cout << "translated front" << std::endl;
				translate(front(), translate_sensitivity);
			}
		});

		Event_center<void>::get_instance()->register_event("key_x_down", 
		[&](){
			if (this->mode == CAMERA_MODE::TRANSLATE) {
				std::cout << "translated back" << std::endl;
				translate(back(), translate_sensitivity);
			}
		});

		Event_center<void, std::pair<int, int>, std::pair<decimal, decimal>>::get_instance()->register_event("on_mouse_move",
		[&](std::pair<int, int> pos, std::pair<decimal, decimal> delta){
			if (this->mode == CAMERA_MODE::ROTATE) {
				std::cout << "rotated " << delta.first << ' ' << delta.second << std::endl;
				rotate(delta.first, delta.second);
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
	decimal delta_x{ 0.0 };
	decimal delta_y{ 0.0 };
	decimal delta_z{ 0.0 };

	decimal translate_sensitivity{ 0.1 };
	decimal rotation_sensitivity{ 1.0 };

	math::Point3d origin_position{};
	math::Vector3d origin_front{};
	math::Vector3d top{};

	CAMERA_MODE mode = CAMERA_MODE::TRANSLATE;

	Camera() = default;
	Camera(decimal fov_, decimal aspect_ratio_, decimal near_, decimal far_, const math::Point3d& front_, const math::Point3d& top_, const math::Point3d& position_) :
	fov(fov_), aspect_ratio(aspect_ratio_), near(near_), far(far_), origin_front(front_), top(top_), origin_position(position_) {
		register_events();
	}

	void rotate(decimal delta_yaw, decimal delta_pitch) {
		yaw -= rotation_sensitivity * delta_yaw, pitch += rotation_sensitivity * delta_pitch; 
	}

	void translate(const math::Vector3d& direction, decimal distance) {
		delta_x += distance * direction.x();
		delta_y += distance * direction.y();
		delta_z += distance * direction.z();
	}

	math::Vector3d front() {
		return math::Vector3d{ math::rotate_y(yaw) * math::rotate_x(pitch) * math::to_homo_vector(origin_front) }.normalize();
	}

	math::Vector3d back() {
		return -front();
	}

	math::Vector3d right() {
		return math::cross(front(), top).normalize();
	}

	math::Vector3d left() {
		return -right();
	}

	math::Vector3d up() {
		return math::cross(right(), front()).normalize();
	}

	math::Vector3d down() {
		return -up();
	}

	math::Point3d position() {
		return { origin_position.x() + delta_x, origin_position.y() + delta_y, origin_position.z() + delta_z }; 
	}

	math::Transform3d get_view_matrix() {
		return math::view(front(), top, position());
	}

	math::Transform3d get_projection_matrix() {
		return math::projection_perspective(fov, aspect_ratio, near, far);
	}

	math::Transform3d get_view_projection_matrix() {
		return get_projection_matrix() * get_view_matrix();
	}

};