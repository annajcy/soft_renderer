#include "application.h"
#include "gpu.h"
#include "raster.h"
#include "maths.h"
#include "base.h"
#include "color.h"

std::string app_id = "soft_renderer";
int height = 400;
int width = 800;
int cnt = 0;

decimal angle = 0.0;
decimal camera_z = 5.0;

math::Homo3d pos1 = math::homo_point(-2.0, 0.0, 0.0);
math::Homo3d pos2 = math::homo_point(2.0, 0.0, 0.0);
math::Homo3d pos3 = math::homo_point(0.0, 2.0, 0.0);

math::Color color1 = math::Color::red();
math::Color color2 = math::Color::green();
math::Color color3 = math::Color::blue();

std::vector<std::pair<math::Pixel, math::Color>> points;

void test() {
	angle += 1.0;
	camera_z += 0.01;

	auto model = math::rotate({0.0, 1.0, 0.0}, angle);
	auto view = math::veiw(math::Transform3d::identity(), math::translate(0.0, 0.0, camera_z));
	auto projection = math::projection_perspective(60.0, (decimal)width / height, 1.0, 10.0);
	auto screen = math::screen(width, height);

	auto clip1 = projection * view * model * pos1;
	auto clip2 = projection * view * model * pos2;
	auto clip3 = projection * view * model * pos3;

	auto point1 = math::normalize_homo_point(clip1);
	auto point2 = math::normalize_homo_point(clip2);
	auto point3 = math::normalize_homo_point(clip3);

	auto screen1 = screen * point1;
	auto screen2 = screen * point2;
	auto screen3 = screen * point3;

	math::Point2d a(math::to_point(screen1));
	math::Point2d b(math::to_point(screen2));
	math::Point2d c(math::to_point(screen3));

	Raster::triangle_colored(points, {a, color1}, {b, color2}, {c, color3});
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