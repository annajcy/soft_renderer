#pragma once

#include "bvh.h"
#include "gpu.h"
#include "mesh.h"
#include "shader_raytracer.h"

namespace raytrace {

	using namespace gpu;

	class Raytracer : public GPU {

	public:

		std::shared_ptr<Shader> shader{};
		std::shared_ptr<rendering::Camera> camera{};

		template<typename T>
		void set_shader(const std::shared_ptr<T> shader_) requires Inherited<Shader, typename std::remove_reference<T>::type> {
			shader = shader_;
		}

		void draw_model(const std::shared_ptr<mesh::Model> &model) override {

			std::vector<std::array<Intermediate_shader_data, 3>> surfaces;

			for (int fid = 0; fid < model->face_count(); fid ++)
			{
				//prepare data
				std::array<Vertex_shader_input_data, 3> face_vs_data{};
				for (int vid : {0, 1, 2}) {
					Vertex_shader_input_data input{};
					input.uv = model->uv(fid, vid);
					input.position = model->position(fid, vid);
					input.normal = model->normal(fid, vid);
					face_vs_data[vid] = input;
				}

				//vertex shade
				std::array<Intermediate_shader_data, 3> intermediate_surface{};
				for (int vid : {0, 1, 2}) {
					intermediate_surface[vid] = shader->vertex_shader(face_vs_data[vid]);
				}

				surfaces.push_back(intermediate_surface);
			}

			std::vector<Triangle> triangles{};
			triangles.reserve(surfaces.size());
			for (int i = 0; i < surfaces.size(); i ++ ) {
				triangles.push_back(Triangle{math::Triangle3d{
					surfaces[i][0].view_position,
					surfaces[i][1].view_position,
					surfaces[i][2].view_position},
											 i});
			}

			auto bvh_root = std::make_shared<BVH_node>(triangles, 5, 50);
			bvh_root->split(0);



		}


	private:

	};

}



