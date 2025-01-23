#pragma once

#include "base.h"
#include "string_helper.h"
#include "bvh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <fstream>

#define ASSIMP_ENABLE

namespace mesh {

	struct Vertex {
		math::Point3d position{};
		math::UV uv{};
		math::Vector3d normal{};
	};

	struct Triangle {
		Vertex a{};
		Vertex b{};
		Vertex c{};

		[[nodiscard]] Vertex interpolate(const std::tuple<decimal, decimal, decimal> &barycentric) const {
			Vertex result{};
			result.position = math::calculate_weighed(a.position, b.position, c.position, barycentric);
			result.uv = math::calculate_weighed(a.uv, b.uv, c.uv, barycentric);
			result.normal = math::calculate_weighed(a.normal, b.normal, c.normal, barycentric);
			return result;
		}

		Vertex& operator[](int index) {
			if (index == 0) return a;
			else if (index == 1) return b;
			else return c;
		}

		Vertex operator[](int index) const {
			if (index == 0) return a;
			else if (index == 1) return b;
			else return c;
		}
	};

	struct Vertex_ref {
		int position_id {-1};
		int uv_id {-1};
		int normal_id {-1};
	};

	struct Triangle_ref {
		Vertex_ref a{};
		Vertex_ref b{};
		Vertex_ref c{};

		Vertex_ref& operator[](int index) {
			if (index == 0) return a;
			else if (index == 1) return b;
			else return c;
		}

		Vertex_ref operator[](int index) const {
			if (index == 0) return a;
			else if (index == 1) return b;
			else return c;
		}
	};

	struct Model {
		std::vector<math::Point3d> positions{};
		std::vector<math::UV> uvs{};
		std::vector<math::Vector3d> normals{};
		std::vector<Triangle_ref> face_refs{};

		explicit Model(const std::string &filename) {

#ifdef ASSIMP_ENABLE

			auto save_mesh_data = [&](const aiMesh* mesh) {
				std::cout << "Mesh: " << mesh->mName.C_Str() << "\n";
				std::cout << "Vertices: " << mesh->mNumVertices << "\n";

				// Print vertex positions, normals, tangents, and UV coordinates
				for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
					auto& v = mesh->mVertices[i];
					std::cout << "  Vertex " << i << ": (" << v.x << ", " << v.y << ", " << v.z << ")";
					positions.push_back(math::Point3d{v.x, v.y, v.z});

					if (mesh->HasNormals()) {
						auto& n = mesh->mNormals[i];
						std::cout << " | Normal: (" << n.x << ", " << n.y << ", " << n.z << ")";
						normals.push_back(math::Vector3d{n.x, n.y, n.z});
					}

					if (mesh->HasTextureCoords(0)) { // Check if the first UV set exists
						auto& uv = mesh->mTextureCoords[0][i]; // Access the first UV set
						std::cout << " | UV: (" << uv.x << ", " << uv.y << ")";
						uvs.push_back(math::UV{uv.x, uv.y});
					}

					std::cout << "\n";
				}

				// Print face indices
				std::cout << "Faces: " << mesh->mNumFaces << "\n";
				for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
					const aiFace& face = mesh->mFaces[i];
					std::cout << "  Face " << i << ":";

					Triangle_ref ref{};
					ref.a.position_id = ref.a.normal_id = ref.a.uv_id = face.mIndices[0];
					ref.b.position_id = ref.b.normal_id = ref.b.uv_id = face.mIndices[1];
					ref.c.position_id = ref.c.normal_id = ref.c.uv_id = face.mIndices[2];

					face_refs.push_back(ref);

					for (unsigned int j = 0; j < face.mNumIndices; ++j) {
						std::cout << " " << face.mIndices[j];
					}
					std::cout << "\n";
				}
			};


			Assimp::Importer importer{};

			const aiScene* scene = importer.ReadFile(
					filename,
					aiProcess_Triangulate |           // Ensure all faces are triangles
					aiProcess_JoinIdenticalVertices |
					aiProcess_GenNormals |
					aiProcess_CalcTangentSpace
			);

			if (scene == nullptr || scene->mRootNode == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
				std::cerr << "Error loading OBJ file: " << importer.GetErrorString() << "\n";
			}

			for (int i = 0; i < scene->mNumMeshes; i ++) {
				const aiMesh* mesh = scene->mMeshes[i];
				save_mesh_data(mesh);
			}

#else
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

#endif
		}

		[[nodiscard]] math::Vector3d normal(int fid, int vid) const {
			auto id = face_refs[fid][vid].normal_id;
			return normals[id];
		}

		[[nodiscard]] math::UV uv(int fid, int vid) const {
			auto id = face_refs[fid][vid].uv_id;
			return uvs[id];
		}

		[[nodiscard]] math::Point3d position(int fid, int vid) const {
			auto id = face_refs[fid][vid].position_id;
			return positions[id];
		}

		[[nodiscard]] Triangle face(int fid) const {
			auto ref = face_refs[fid];
			Triangle result{};
			for (int i : {0, 1, 2}) {
				result[i].position = positions[ref[i].position_id];
				result[i].uv = uvs[ref[i].uv_id];
				result[i].normal = normals[ref[i].normal_id];
			}
			return result;
		}

		[[nodiscard]] size_t face_count() const {
			return face_refs.size();
		}

	};

}

