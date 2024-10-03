#include "application.h"
#include "gpu.h"
#include "raster.h"
#include "maths.h"
#include "base.h"
#include "color.h"
#include "camera.h"

std::string app_id = "soft_renderer";
int height = 300;
int width = 400;

decimal angle = 0.0;
decimal camera_z = 5.0;

Camera camera(70.0, (decimal)width / height, -0.5, -1000.0, {0.0, 0.0, 1.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, -camera_z});

decimal positions[] = {
			-1.5, 0.0, 0.0,
			1.5, 0.0, 0.0,
			0.0, 5.0, 0.0,
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

void render() {
	angle += 0.1;

	auto model = math::rotate({0.0, 1.0, 0.0}, angle);
	auto view = camera.get_view_matrix();
	auto projection = camera.get_projection_matrix();

	Default_Shader default_Shader(model, view, projection);
	gpu->set_shader(default_Shader);

	int ebo = gpu->generate(OBJECT::ELEMENT_BUFFER);
	gpu->bind(OBJECT::ELEMENT_BUFFER, ebo);
	gpu->set_buffer(OBJECT::ELEMENT_BUFFER, indices, sizeof(indices) / sizeof(int));

	int vertex_vao = gpu->generate(OBJECT::VERTEX_ARRAY);
	gpu->bind(OBJECT::VERTEX_ARRAY, vertex_vao);
	gpu->set_vertex_array({1, 3, 0, 3});

	int color_vao = gpu->generate(OBJECT::VERTEX_ARRAY);
	gpu->bind(OBJECT::VERTEX_ARRAY, color_vao);
	gpu->set_vertex_array({2, 4, 0, 4});

	int uv_vao = gpu->generate(OBJECT::VERTEX_ARRAY);
	gpu->bind(OBJECT::VERTEX_ARRAY, uv_vao);
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

	gpu->draw_primitive(PRIMITIVE::TRIANGLE);

	gpu->bind(OBJECT::VERTEX_BUFFER, 0);
	gpu->bind(OBJECT::ELEMENT_BUFFER, 0);
	gpu->bind(OBJECT::VERTEX_ARRAY, 0);

}

int main()
{
	gpu->init(width, height);
	app->init(width, height, app_id, gpu->color_buffer());

	while (app->active) {
		gpu->clear();

		render();

		app->update();
		app->handle_message();
	}

	app->exit();

	return 0;
}