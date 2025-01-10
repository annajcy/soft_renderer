#pragma once

#include "base.h"

namespace utils
{
	inline std::vector<std::string> string_split(const std::string &input, char delimiter) {
		std::vector<std::string> result;
		std::istringstream stream(input);
		std::string token;

		while (std::getline(stream, token, delimiter)) {
			result.push_back(token);
		}

		return result;
	}
}

