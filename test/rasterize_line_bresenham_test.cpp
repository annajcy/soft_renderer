#include "application.h"
#include "gpu.h"
#include "raster.h"
#include "maths.h"
#include "base.h"

std::string app_id = "soft_renderer";
int height = 400;
int width = 800;
int cnt = 0;

int main()
{
    app->init(width, height, app_id);
	gpu->init(app->canvas);

	std::vector<math::Point2di> line;
	Raster::rasterize_line_bresenham(line, {100, 100}, {200, 300});

    while (app->active) {

	    gpu->clear();

	    for (auto &p : line) {
		    gpu->set_pixel(p.x(), p.y(), Color());
		    std::cout << p.x() << ' ' << p.y() << std::endl;
	    }

	    for (auto &p : line) {
		    gpu->set_pixel(p.x(), p.y(), Color());
		    std::cout << p.x() << ' ' << p.y() << ' ' << std::endl;
	    }

        app->update();
        auto message = app->get_message();
        app->handle_message(message);
    }

	app->exit();

    return 0;
}