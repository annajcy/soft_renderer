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


}


