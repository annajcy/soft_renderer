#pragma once

#include "base.h"
#include "string_helper.h"
#include <fstream>

namespace mesh {

	struct Vertex_ref {
		int position_id {-1};
		int uv_id {-1};
		int normal_id {-1};
	};

	struct Face_ref {
		Vertex_ref a{};
		Vertex_ref b{};
		Vertex_ref c{};

		Vertex_ref& operator[](int index) {
			if (index == 0) return a;
			else if (index == 1) return b;
			else return c;
		}
	};

	struct Model {
		std::vector<math::Point3d> positions{};
		std::vector<math::UV> uvs{};
		std::vector<math::Vector3d> normals{};
		std::vector<Face_ref> face_refs{};

		explicit Model(const std::string &filename) {
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
					positions.push_back(math::Point3d{x, y, z});
				} else if (prefix == "vt") {
					decimal u, v;
					iss >> u >> v;
					uvs.push_back(math::UV{u, v});
				} else if (prefix == "vn") {
					decimal x, y, z;
					iss >> x >> y >> z;
					normals.push_back(math::Point3d{x, y, z});
				} else if (prefix == "f") {
					std::string s;
					Face_ref face_ref{};
					for (int i = 0; iss >> s; i ++) {
						auto values = utils::string_split(s, '/');
						if (!values.empty()) face_ref[i].position_id = stoi(values[0]) - 1;
						if (values.size() >= 2) face_ref[i].uv_id = stoi(values[1]) - 1;
						if (values.size() >= 3) face_ref[i].normal_id = stoi(values[2]) - 1;
					}
					face_refs.push_back(face_ref);
				}
			}
		}

		[[nodiscard]] math::Vector3d normal(int fid, int vid) {
			auto id = face_refs[fid][vid].normal_id;
			return normals[id];
		}

		[[nodiscard]] math::UV uv(int fid, int vid) {
			auto id = face_refs[fid][vid].uv_id;
			return uvs[id];
		}

		[[nodiscard]] math::Point3d position(int fid, int vid) {
			auto id = face_refs[fid][vid].position_id;
			return positions[id];
		}

		[[nodiscard]] size_t face_count() const {
			return face_refs.size();
		}

	};

}

