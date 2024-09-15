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

	math::Point2d a{20.0, 20.0}, b{80.0, 80.0}, c{20.0, 160.0};

	std::vector<std::pair<math::Pixel, Color>> points;
	Raster::rasterize_traingle_colored(points, {a, Color(255, 0, 0)}, {b, Color(0, 255, 0)}, {c, Color(0, 0, 255)}, 4);
	
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