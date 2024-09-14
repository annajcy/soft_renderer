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

int main()
{

	math::Vec<double, 3> vec1 = {1.00000001, 2.0, 3.0};
    math::Vec<double, 3> vec2 = {1.00000002, 2.0, 3.0};
    math::Vec<int, 3> vec3 = {1, 2, 3};
    math::Vec<int, 3> vec4 = {1, 2, 3};

    if (vec1 == vec2) {
        std::cout << "vec1 and vec2 are equal." << std::endl;
    } else {
        std::cout << "vec1 and vec2 are not equal." << std::endl;
    }

    if (vec3 == vec4) {
        std::cout << "vec3 and vec4 are equal." << std::endl;
    } else {
        std::cout << "vec3 and vec4 are not equal." << std::endl;
    }

    app->init(width, height, app_id);
	gpu->init(app->canvas);

	std::vector<math::Pixel_color> line;
	Raster::rasterize_line_bresenham_colored(line, {{100, 100}, Color(255, 0, 0, 255)}, {{300, 200}, Color(0, 255, 0, 255)});

    while (app->active) {

	    gpu->clear();

	    for (auto &[p, color] : line) {
		    gpu->set_pixel(p.x(), p.y(), color);
		    std::cout << p.x() << ' ' << p.y() << color << std::endl;
	    }

        app->update();
        auto message = app->get_message();
        app->handle_message(message);
    }

	app->exit();

    return 0;
}