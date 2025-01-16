#pragma once

#include <utility>

#include "base.h"
#include "maths.h"
#include "event_center.h"

namespace rendering {

	using namespace framework;

	enum CAMERA_MOVE_MODE{
		TRANSLATE,
		ROTATE
	};

	class Camera {
	private:

		void register_events() {

			Event_center<void>::get_instance()->register_event("key_w_down",
					[&](){
						if (this->mode == CAMERA_MOVE_MODE::TRANSLATE) {
							std::cout << "translated up" << std::endl;
							translate(up(), translate_sensitivity);
						}
					});

			Event_center<void>::get_instance()->register_event("key_s_down",
					[&](){
						if (this->mode == CAMERA_MOVE_MODE::TRANSLATE) {
							std::cout << "translated down" << std::endl;
							translate(down(), translate_sensitivity);
						}
					});

			Event_center<void>::get_instance()->register_event("key_a_down",
					[&](){
						if (this->mode == CAMERA_MOVE_MODE::TRANSLATE) {
							std::cout << "translated left" << std::endl;
							translate(left(), translate_sensitivity);
						}
					});

			Event_center<void>::get_instance()->register_event("key_d_down",
					[&](){
						if (this->mode == CAMERA_MOVE_MODE::TRANSLATE) {
							std::cout << "translated right" << std::endl;
							translate(right(), translate_sensitivity);
						}
					});

			Event_center<void>::get_instance()->register_event("key_z_down",
					[&](){
						if (this->mode == CAMERA_MOVE_MODE::TRANSLATE) {
							std::cout << "translated front" << std::endl;
							translate(front(), translate_sensitivity);
						}
					});

			Event_center<void>::get_instance()->register_event("key_x_down",
					[&](){
						if (this->mode == CAMERA_MOVE_MODE::TRANSLATE) {
							std::cout << "translated back" << std::endl;
							translate(back(), translate_sensitivity);
						}
					});

			Event_center<void, std::pair<int, int>, std::pair<decimal, decimal>>::get_instance()->register_event("on_mouse_move",
					[&](std::pair<int, int> pos, std::pair<decimal, decimal> delta){
						if (this->mode == CAMERA_MOVE_MODE::ROTATE) {
							std::cout << "rotated " << delta.first << ' ' << delta.second << std::endl;
							rotate(delta.first, delta.second);
						}
					});

			Event_center<void, std::pair<int, int>, std::pair<decimal, decimal>>::get_instance()->register_event("on_mouse_left_button_up",
					[&](std::pair<int, int> pos, std::pair<decimal, decimal> delta){
						this->mode = CAMERA_MOVE_MODE::TRANSLATE;
						std::cout << "mode changed: translate" << std::endl;
					});

			Event_center<void, std::pair<int, int>, std::pair<decimal, decimal>>::get_instance()->register_event("on_mouse_left_button_down",
					[&](std::pair<int, int> pos, std::pair<decimal, decimal> delta){
						this->mode = CAMERA_MOVE_MODE::ROTATE;
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

		CAMERA_MOVE_MODE mode = CAMERA_MOVE_MODE::TRANSLATE;

		Camera() = default;
		Camera(decimal fov_, decimal aspect_ratio_, decimal near_, decimal far_, math::Point3d  front_, math::Point3d  top_, math::Point3d  position_) :
				fov(fov_), aspect_ratio(aspect_ratio_), near(near_), far(far_), origin_front(std::move(front_)), top(std::move(top_)), origin_position(std::move(position_)) {
			register_events();
		}

		void place_at(const math::Point3d &place_at_point) {
			delta_x = place_at_point.x() - position().x();
			delta_y = place_at_point.y() - position().y();
			delta_z = place_at_point.z() - position().z();
		}

		void look_at(const math::Point3d &look_at_point) {
			math::Vector3d direction = (look_at_point - position()).normalize();
			decimal new_yaw = std::atan2(direction.z(), direction.x());
			decimal new_pitch = std::asin(direction.y());
			decimal delta_yaw = new_yaw - yaw;
			decimal delta_pitch = new_pitch - pitch;
			rotate(delta_yaw, delta_pitch);
		}

		void rotate(decimal delta_yaw, decimal delta_pitch) {
			yaw += rotation_sensitivity * delta_yaw, pitch += rotation_sensitivity * delta_pitch;
		}

		void translate(const math::Vector3d& direction, decimal distance) {
			delta_x += distance * direction.x();
			delta_y += distance * direction.y();
			delta_z += distance * direction.z();
		}

		[[nodiscard]] math::Vector3d front() const {
			return math::Vector3d{ math::rotate_y(yaw) * math::rotate_x(pitch) * math::to_homo_vector(origin_front) }.normalize();
		}

		[[nodiscard]] math::Vector3d back() const {
			return -front();
		}

		[[nodiscard]] math::Vector3d right() const {
			return math::cross(front(), top).normalize();
		}

		[[nodiscard]] math::Vector3d left() const {
			return -right();
		}

		[[nodiscard]] math::Vector3d up() const {
			return math::cross(right(), front()).normalize();
		}

		[[nodiscard]] math::Vector3d down() const {
			return -up();
		}

		[[nodiscard]] math::Point3d position() const {
			return { origin_position.x() + delta_x, origin_position.y() + delta_y, origin_position.z() + delta_z };
		}

		[[nodiscard]] math::Transform3d get_view_matrix() const {
			return math::view(front(), top, position());
		}

		[[nodiscard]] math::Transform3d get_projection_matrix() const {
			return math::projection_perspective(fov, aspect_ratio, near, far);
		}

	};
}

