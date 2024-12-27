#include "application.h"
#include "gpu.h"
#include "raster.h"
#include "maths.h"
#include "base.h"
#include "color.h"
#include "camera.h"
#include "model.h"

using namespace application;
using namespace gpu;
using namespace rendering;
using namespace model_loader;

std::string app_id = "soft_renderer";
int height = 300;
int width = 400;

decimal angle = 0.0;
decimal camera_z = 5.0;

std::string model_path = "assets/obj/spot/spot_triangulated_good.obj";
std::string texture_path = "assets/obj/spot/spot_texture.png";

Camera camera(
		70.0,
		(decimal)width / height,
		-0.5,
		-1000.0,
		{0.0, 0.0, 1.0},
		{0.0, 1.0, 0.0},
		{0.0, 0.0, -camera_z});

Lightenings lightenings;
Texture main_texture(texture_path);
Model model(model_path);


//decimal positions[] = {
//	-3.0, 0.0, 0.0,
//	3.0, 0.0, 0.0,
//	0.0, 5.0, 0.0,
//};
//
//decimal colors[] = {
//	1.0, 0.0, 0.0, 1.0,
//	0.0, 1.0, 0.0, 1.0,
//	0.0, 0.0, 1.0, 1.0,
//};
//
//decimal uvs[] = {
//	0.0, 0.0,
//	0.0, 1.0,
//	1.0, 0.0,
//};
//
//int indices[] = { 0, 1, 2 };

void load() {
	
//	int ebo = GPU::get_instance()->generate(OBJECT::ELEMENT_BUFFER);
//	GPU::get_instance()->bind(OBJECT::ELEMENT_BUFFER, ebo);
//	GPU::get_instance()->set_ebo(indices, sizeof(indices) / sizeof(int));
//
//	int vertex_vao = GPU::get_instance()->generate(OBJECT::VERTEX_ARRAY);
//	GPU::get_instance()->bind(OBJECT::VERTEX_ARRAY, vertex_vao);
//	GPU::get_instance()->set_vao({1, 3, 0, 3});
//
//	int color_vao = GPU::get_instance()->generate(OBJECT::VERTEX_ARRAY);
//	GPU::get_instance()->bind(OBJECT::VERTEX_ARRAY, color_vao);
//	GPU::get_instance()->set_vao({2, 4, 0, 4});
//
//	int uv_vao = GPU::get_instance()->generate(OBJECT::VERTEX_ARRAY);
//	GPU::get_instance()->bind(OBJECT::VERTEX_ARRAY, uv_vao);
//	GPU::get_instance()->set_vao({3, 2, 0, 2});
//
//	int position_vbo = GPU::get_instance()->generate(OBJECT::VERTEX_BUFFER);
//	GPU::get_instance()->bind(OBJECT::VERTEX_BUFFER, position_vbo);
//	GPU::get_instance()->set_vbo(positions, sizeof(positions) / sizeof(decimal));
//
//	int color_vbo = GPU::get_instance()->generate(OBJECT::VERTEX_BUFFER);
//	GPU::get_instance()->bind(OBJECT::VERTEX_BUFFER, color_vbo);
//	GPU::get_instance()->set_vbo(colors, sizeof(colors) / sizeof(decimal));
//
//	int uv_vbo = GPU::get_instance()->generate(OBJECT::VERTEX_BUFFER);
//	GPU::get_instance()->bind(OBJECT::VERTEX_BUFFER, uv_vbo);
//	GPU::get_instance()->set_vbo(uvs, sizeof(uvs) / sizeof(decimal));
}

void render() {
	angle += 0.1;

	auto model_mat = math::rotate({0.0, 1.0, 0.0}, angle);
	auto view_mat = camera.get_view_matrix();
	auto projection_mat = camera.get_projection_matrix();

	Default_Shader default_shader(model_mat, view_mat, projection_mat, &camera, &lightenings, &main_texture);
	GPU::get_instance()->set_shader(default_shader);

	GPU::get_instance()->draw_primitive(PRIMITIVE::TRIANGLE);
}

int main()
{
	GPU::get_instance()->init(width, height);
	Application::get_instance()->init(width, height, app_id, GPU::get_instance()->color_buffer());

	load();

	while (Application::get_instance()->active) {
		GPU::get_instance()->clear();

		render();

		Application::get_instance()->update();
		Application::get_instance()->handle_message();
	}

	Application::exit();

	return 0;
}