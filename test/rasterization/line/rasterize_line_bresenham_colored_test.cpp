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
    app->init(width, height, app_id);
	gpu->init(app->canvas);

	std::vector<math::Pixel_color> line;
	Raster::line_bresenham_colored(line, {{100, 100}, Color(255, 0, 0, 255)}, {{300, 200}, Color(0, 255, 0, 255)});

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