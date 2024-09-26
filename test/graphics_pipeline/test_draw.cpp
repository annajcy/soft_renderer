#include "application.h"
#include "gpu.h"
#include "raster.h"
#include "maths.h"
#include "base.h"
#include "color.h"

std::string app_id = "soft_renderer";
int height = 300;
int width = 400;

decimal angle = 0.0;
decimal camera_z = 5.0;

math::Homo3d pos1 = math::homo_point(-1.5, 0.0, 0.0);
math::Homo3d pos2 = math::homo_point(1.5, 0.0, 0.0);
math::Homo3d pos3 = math::homo_point(0.0, 2.0, 0.0);

math::Color color1 = math::Color::red();
math::Color color2 = math::Color::green();
math::Color color3 = math::Color::blue();

auto model = math::rotate({0.0, 1.0, 0.0}, angle);
auto view = math::view({0.0, 0.0, 1.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, -camera_z});
auto projection = math::projection_perspective(60.0, (decimal)width / height, -0.1, -100.0);
auto screen = math::screen(width, height);

std::vector<std::pair<math::Pixel, math::Color>> points;

void test() {
	decimal positions[] = {
			-1.5, 0.0, 0.0,
			1.5, 0.0, 0.0,
			0.0, 2.0, 0.0,
	};

	decimal colors[] = {
			1.0, 0.0, 0.0, 1.0,
			0.0, 1.0, 0.0, 1.0,
			0.0, 0.0, 1.0, 1.0,
	};

	decimal uvs[] = {
			0.0, 0.0,
			0.0, 1.0,
			1.0, 0.0,
	};

	int indices[] = { 0, 1, 2 };

	Default_Shader default_Shader(model, view, projection);
	gpu->set_shader(default_Shader);

	int ebo = gpu->generate(OBJECT::ELEMENT_BUFFER);
	gpu->bind(OBJECT::ELEMENT_BUFFER, ebo);
	gpu->set_buffer(OBJECT::ELEMENT_BUFFER, indices, sizeof(indices) / sizeof(int));

	int vao;

	vao = gpu->generate(OBJECT::VERTEX_ARRAY);
	gpu->bind(OBJECT::VERTEX_ARRAY, vao);
	gpu->set_vertex_array({1, 3, 0, 3});

	vao = gpu->generate(OBJECT::VERTEX_ARRAY);
	gpu->bind(OBJECT::VERTEX_ARRAY, vao);
	gpu->set_vertex_array({2, 4, 0, 4});

	vao = gpu->generate(OBJECT::VERTEX_ARRAY);
	gpu->bind(OBJECT::VERTEX_ARRAY, vao);
	gpu->set_vertex_array({3, 2, 0, 2});

	int position_vbo = gpu->generate(OBJECT::VERTEX_BUFFER);
	gpu->bind(OBJECT::VERTEX_BUFFER, position_vbo);
	gpu->set_buffer(OBJECT::VERTEX_BUFFER, positions, sizeof(positions) / sizeof(decimal));

	int color_vbo = gpu->generate(OBJECT::VERTEX_BUFFER);
	gpu->bind(OBJECT::VERTEX_BUFFER, color_vbo);
	gpu->set_buffer(OBJECT::VERTEX_BUFFER, colors, sizeof(colors) / sizeof(decimal));

	int uv_vbo = gpu->generate(OBJECT::VERTEX_BUFFER);
	gpu->bind(OBJECT::VERTEX_BUFFER, uv_vbo);
	gpu->set_buffer(OBJECT::VERTEX_BUFFER, uvs, sizeof(uvs) / sizeof(decimal));


//	gpu->bind(OBJECT::VERTEX_BUFFER, 0);
//	gpu->bind(OBJECT::ELEMENT_BUFFER, 0);
//	gpu->bind(OBJECT::VERTEX_ARRAY, 0);

	gpu->draw_triangle();
	// gpu->print_state();

}

int main()
{
	gpu->init(width, height);
	app->init(width, height, app_id, gpu->color_buffer());

	test();

	while (app->active) {

		gpu->clear();

		for (auto &[pixel, color] : points) {
			gpu->set_pixel(pixel.x(), pixel.y(), color);
		}

		app->update();
		auto message = app->get_message();
		app->handle_message(message);
	}

	app->exit();

	return 0;
}