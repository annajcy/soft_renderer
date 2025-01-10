#include "application.h"
#include "gpu.h"
#include "raster.h"
#include "maths.h"
#include "base.h"
#include "camera.h"
#include "mesh.h"

using namespace application;
using namespace gpu;
using namespace rendering;
using namespace mesh;

std::string app_id = "soft_renderer";
int height = 700;
int width = 700;

decimal angle = -40;
decimal camera_z = 10.0;

std::string model_path = "assets/obj/spot/spot_triangulated_good.obj";
std::string main_texture_path = "assets/obj/spot/spot_texture.png";
std::string displacement_texture_path = "assets/obj/spot/hmap.jpg";

auto model = std::make_shared<Model>(model_path);
auto textures = std::make_shared<Texture_set>(std::unordered_map<std::string, std::shared_ptr<application::Texture>> {
		{"main", std::make_shared<Texture>(main_texture_path)},
		{"height", std::make_shared<Texture>(displacement_texture_path)}
});

auto camera = std::make_shared<Camera>(
		45.0,
		(decimal) width / height,
		-0.1,
		-50.0,
		math::Vector3d {0.0, 0.0, 1.0},
		math::Vector3d {0.0, 1.0, 0.0},
		math::Vector3d {0.0, 0.0, -camera_z}
);

auto lighting = std::make_shared<Lighting>(
		std::vector<Point_light> {
			Point_light{math::Color::white(), {10.0, 10.0, 10.0}, {500, 500, 500}},
			Point_light{math::Color::white(), {-10.0, 10.0, 10.0}, {500, 500, 500}},
		},
		std::vector<Directional_light> {
			Directional_light(math::Color::white(), {1.0, 1.0, 1.0}, {500, 500, 500})
		},
		std::vector<Ambient_light> {
			Ambient_light {math::Color::white(), {10, 10, 10}}
		}
);

void render() {
	angle += 0.2;

	auto model_mat = math::rotate({0.0, 1.0, 0.0}, angle) * math::scale(2.5, 2.5, 2.5);
	auto view_mat = camera->get_view_matrix();
	auto projection_mat = camera->get_projection_matrix();

	auto blinn_phong_Shader = std::make_shared<Blinn_Phong_Shader>(model_mat, view_mat, projection_mat, camera, lighting, textures);

    GPU::get_instance()->set_shader(blinn_phong_Shader);
	GPU::get_instance()->draw_model(model);

}

int main()
{
	GPU::get_instance()->init(width, height);
	Application::get_instance()->init(width, height, app_id, GPU::get_instance()->color_buffer_raw());

	while (Application::get_instance()->active) {
		GPU::get_instance()->clear();

		render();

		Application::get_instance()->update();
		Application::get_instance()->handle_message();
	}

	Application::exit();

	return 0;
}