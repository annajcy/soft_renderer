#pragma once

#include "image.h"

namespace rendering {

	struct Texture_set {

	private:
		std::unordered_map<std::string, std::shared_ptr<application::Texture>> textures{};
	public:
		explicit Texture_set(std::unordered_map<std::string, std::shared_ptr<application::Texture>> &&textures_) :
				textures(std::move(textures_)) {}

		explicit Texture_set(const std::unordered_map<std::string, std::shared_ptr<application::Texture>> &textures_) :
				textures(textures_) {}

		std::shared_ptr<application::Texture>& get_texture(const std::string &identifier) {
			return textures[identifier];
		}

	};

	struct Material {

		enum class Material_type {
			DIFFUSE_AND_GLOSSY,
			REFRACTION_ONLY,
			REFLECTION_ONLY,
		};

		Material_type type{Material_type::DIFFUSE_AND_GLOSSY };
		math::Vector3d base_color{};
		math::Vector3d emission_color{};
		decimal ior{1.33};
		decimal kd{10};
		decimal ks{5};
		decimal specular_exp{50.0};
		decimal reflection_loss{0.0};
		std::shared_ptr<Texture_set> texture_set{};

		Material(const std::shared_ptr<Texture_set> &texture_set_) : texture_set(texture_set_) {}

		[[nodiscard]] math::Vector3d get_uv_color(const std::string& texture_name, decimal u, decimal v) const {

			if (!texture_set) {
				std::cout << "Texture set does not exist" << std::endl;
				return math::Vector3d::zeros();
			}

			return cast_dims<3>(texture_set->get_texture(texture_name)->at_uv_bilinear(u, v).to_color_decimal());
		}

	};




}


