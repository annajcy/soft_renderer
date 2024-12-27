#pragma once

#include "base.h"
#include <fstream>

namespace model_loader{

	struct Model {

		std::vector<decimal> vertices{};
		std::vector<decimal> uvs{};
		std::vector<decimal> normals{};
		std::vector<std::vector<int>> faces{};
		std::unordered_map<int, int> vertex_to_uv{};
		std::unordered_map<int, int> vertex_to_normal{};

		explicit Model(const std::string &filename) : faces(3) {
			std::ifstream file(filename);

			if (!file.is_open()) {
				std::cerr << "Failed to open file: " << filename << std::endl;
				return;
			}

			std::string line;
			while (std::getline(file, line)) {
				std::istringstream iss(line);
				std::string prefix;
				iss >> prefix;

				if (prefix == "v") {
					decimal x, y, z;
					iss >> x >> y >> z;
					vertices.push_back(x), vertices.push_back(y), vertices.push_back(z);
				} else if (prefix == "vt") {
					decimal u, v;
					iss >> u >> v;
				} else if (prefix == "vn") {
					decimal x, y, z;
					iss >> x >> y >> z;
				} else if (prefix == "f") {
					std::string data;
					while (iss >> data) {
						int val = 0, cnt = 0;
						int vert_val = 0;
						for (auto &c : data) {
							if (isdigit(c)) val *= 10, val += (c - '0');
							else {
								faces[cnt].push_back(val);
								if (cnt == 0) vert_val = val;
								else if (cnt == 1) vertex_to_uv[vert_val] = val;
								else if (cnt == 2) vertex_to_normal[vert_val] = val;
								val = 0, cnt ++;
							}
						}
					}
				}
			}
		}

		decimal *get_vertices() { return vertices.data(); }
		decimal *get_uvs() { return uvs.data(); }
		decimal *get_normals() { return normals.data(); }
		int* get_face_vertices_id() { return faces[0].data(); };
		int* get_face_uvs_id() { return faces[1].data(); };
		int* get_face_normals_id() { return faces[2].data(); };

	};

}

