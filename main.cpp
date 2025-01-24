#include "application.h"
#include "rasterizer.h"
#include "raytracer.h"
#include "maths.h"
#include "base.h"
#include "camera.h"
#include "mesh.h"
#include "curve.h"
#include "singleton.h"

using namespace application;
using namespace rendering;
using namespace mesh;
using namespace raytrace;

using rtr = Singleton<Raytracer>;
using app = Singleton<Application>;

std::string app_id = "soft_renderer";
int height = 500;
int width = 500;

decimal angle = 45;
decimal camera_z = 10.0;

//std::string model_path = "assets/obj/cube/cube.obj";
std::string model_path = "assets/obj/spot/spot_triangulated_good.obj";
std::string main_texture_path = "assets/obj/spot/spot_texture.png";
std::string displacement_texture_path = "assets/obj/spot/hmap.jpg";

auto model = std::make_shared<mesh::Model>(model_path);
auto textures = std::make_shared<Texture_set>(std::unordered_map<std::string, std::shared_ptr<application::Texture>> {
		{"main", std::make_shared<Texture>(main_texture_path)},
		{"height", std::make_shared<Texture>(displacement_texture_path)}
});

auto material = std::make_shared<Material>(textures);

auto camera = std::make_shared<Camera>(
		60,
		(decimal)width,
		(decimal)height,
		-0.1,
		-50.0,
		math::Vector3d {0.0, 0.0, 1.0},
		math::Vector3d {0.0, 1.0, 0.0},
		math::Vector3d {0.0, 0.0, -camera_z}
);

auto pl1 = std::make_shared<Point_light>(math::Color::red(), math::Vector3d {50, 50, 50}, math::Point3d {10.0, 10.0, 10.0});
auto pl2 = std::make_shared<Point_light>(math::Color::green(), math::Vector3d {50, 50, 50}, math::Point3d {-10.0, 10.0, 10.0});
auto pl3 = std::make_shared<Point_light>(math::Color::blue(), math::Vector3d {50, 50, 50}, math::Point3d {0.0, 10.0, 0.0});
auto dl1 = std::make_shared<Directional_light>(math::Color::white(), math::Vector3d {0.5, 0.5, 0.5}, math::Point3d {1.0, 1.0, 1.0});
auto dl2 = std::make_shared<Directional_light>(math::Color::white(), math::Vector3d {0.5, 0.5, 0.5}, math::Point3d {-1.0, 1.0, 1.0});
auto al = std::make_shared<Ambient_light> (math::Color::white(), math::Vector3d {10, 10, 10});

auto lighting = std::make_shared<Lighting>(
		std::vector<std::shared_ptr<Light>> {
			pl1, pl2, pl3, dl1, dl2, al
		}
);

auto scene = std::make_shared<Scene>();

void prepare() {

	BVH_node::max_depth = 100;
	BVH_node::max_primitives_count = 10;
	rtr::get_instance()->max_depth = 1;

	scene->set_light(lighting);
	scene->set_camera(camera);
	auto model_mat = math::rotate({0.0, 1.0, 0.0}, angle) * math::scale(4.0, 4.0, 4.0);
	auto view_mat = scene->camera->get_view_matrix();
	scene->add_model(std::make_shared<rendering::Model>(*model, view_mat * model_mat, material));

	auto blinn_phong_Shader = std::make_shared<Blinn_phong_shader>();
	blinn_phong_Shader->scene = scene;

	rtr::get_instance()->set_shader(blinn_phong_Shader);
}

void render() {
	rtr::get_instance()->render_scene(scene);
}

int main()
{
	rtr::get_instance()->init(width, height);
	app::get_instance()->init(width, height, app_id, rtr::get_instance()->color_buffer_raw());
	app::get_instance()->delta_time = 0;

	prepare();

	while (app::get_instance()->active) {
		rtr::get_instance()->clear();

		render();

		app::get_instance()->update();
		app::get_instance()->handle_message();
	}

	Application::exit();

	return 0;
}

