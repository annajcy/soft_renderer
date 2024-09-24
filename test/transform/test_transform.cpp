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

std::vector<std::pair<math::Pixel, math::Color>> points;

void test() {

	auto mirror = math::mirror({1.0, 0.0, 0.0});
	auto model = math::rotate({0.0, 1.0, 0.0}, angle);
	auto view = math::view({0.0, 0.0, 1.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, -camera_z});
	auto projection = math::projection_perspective(60.0, (decimal)width / height, -0.1, -100.0);
	auto screen = math::screen(width, height);

	std::cout << "pos:\n" << std::endl;
	std::cout << pos1 << ' ' << pos2 << ' ' << pos3 << std::endl;

	auto mirror1 = mirror * pos1;
	auto mirror2 = mirror * pos2;
	auto mirror3 = mirror * pos3;

	std::cout << "mirror:\n" << std::endl;
	std::cout << mirror1 << ' ' << mirror2 << ' ' << mirror3 << std::endl;

	auto model1 = model * pos1;
	auto model2 = model * pos2;
	auto model3 = model * pos3;

//	std::cout << "model:\n" << std::endl;
//	std::cout << model1 << ' ' << model2 << ' ' << model3 << std::endl;

	auto view1 = view * model1;
	auto view2 = view * model2;
	auto view3 = view * model3;

//	std::cout << "view:\n" << std::endl;
//	std::cout << view1 << ' ' << view2 << ' ' << view3 << std::endl;

	auto clip1 = projection * view1;
	auto clip2 = projection * view2;
	auto clip3 = projection * view3;

//	std::cout << "projection(clip space):\n" << std::endl;
//	std::cout << clip1 << ' ' << clip2 << ' ' << clip3 << std::endl;

	auto point1 = math::normalize_homo_point(clip1);
	auto point2 = math::normalize_homo_point(clip2);
	auto point3 = math::normalize_homo_point(clip3);

//	std::cout << "ndc space:\n" << std::endl;
//	std::cout << point1 << ' ' << point2 << ' ' << point3 << std::endl;

	auto screen1 = screen * point1;
	auto screen2 = screen * point2;
	auto screen3 = screen * point3;

	math::Point2d a(math::to_point(screen1));
	math::Point2d b(math::to_point(screen2));
	math::Point2d c(math::to_point(screen3));

//	std::cout << "screen space:\n" << std::endl;
//	std::cout << a << ' ' << b << ' ' << c << std::endl;

	Raster::triangle_colored(points, {a, color1}, {b, color2}, {c, color3});

	angle += 1.0;
	//camera_z += 0.01;
}

int main()
{
	gpu->init(width, height);
	app->init(width, height, app_id, gpu->color_buffer());

	while (app->active) {

		gpu->clear();

		test();

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