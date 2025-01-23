#pragma once

#include "bvh.h"
#include "mesh.h"

namespace rendering {

	struct Model {
		math::Transform3d model_view_mat{};
		std::vector<std::shared_ptr<Primitive>> primitives{};
		std::shared_ptr<Material> material{};

		Model() = default;
		explicit Model(const mesh::Model &mesh_model, const std::shared_ptr<Material> &material_) : model_view_mat(math::Transform3d::identity()), material(material_) {
			for (int i = 0; i < mesh_model.face_count(); i ++) {
				auto face = mesh_model.face(i);
				auto primitive = std::make_shared<Triangle>(face, material);
				primitives.push_back(primitive);
			}
		}

		Model(const mesh::Model &mesh_model, const math::Transform3d& model_view_mat_, const std::shared_ptr<Material> &material_) : model_view_mat(model_view_mat_), material(material_) {
			for (int i = 0; i < mesh_model.face_count(); i ++) {
				auto face = mesh_model.face(i);
				auto primitive = std::make_shared<Triangle>(face, material);
				for (int j : {0, 1, 2}) {
					primitive->triangle[j].position = to_point((model_view_mat_ * to_homo_point(primitive->triangle[j].position)));
					primitive->triangle[j].normal = to_point((model_view_mat_.inv().transpose() * to_homo_point(primitive->triangle[j].normal)));
				}
				primitives.push_back(primitive);
			}
		}

		template<typename T>
		void add_primitive(const std::shared_ptr<T> &primitive) requires Inherited<Primitive, T> {
			add_primitive(primitive);
		}

		[[nodiscard]] math::Transform3d get_model_view_mat() const {
			return model_view_mat;
		}

	};
}