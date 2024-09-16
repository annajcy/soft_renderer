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

	std::vector<math::Pixel_alpha> line;
	Raster::line_alpha(line, {100, 100}, {200, 300});

    while (app->active) {

	    gpu->clear();

	    for (auto &[p, alpha] : line) {
			gpu->set_pixel(p.x(), p.y(), Color(alpha));
			std::cout << p.x() << ' ' << p.y() << ' ' << alpha << std::endl;
		}

        app->update();
        auto message = app->get_message();
        app->handle_message(message);
    }

	app->exit();

    return 0;
}