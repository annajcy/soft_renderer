#include "application.h"
#include "gpu.h"
#include "raster.h"
#include "maths.h"
#include "base.h"
#include "color.h"
#include "image.h"

std::string app_id = "soft_renderer";
int height = 800;
int width = 1600;
int cnt = 0;

int main()
{
    app->init(width, height, app_id);
	gpu->init(app->canvas);

	Image img("assets/img1.png");

	std::vector<std::pair<math::Pixel, math::Color>> v;
	Raster::image(v, img, {0, 0}, width, height, true, WRAP_MODE::MIRROR, FILL_MODE::FIT_HEIGHT);
	
    while (app->active) {

	    gpu->clear();

		for (auto &[pixel, color] : v) {
			gpu->set_pixel(pixel.x(), pixel.y(), color);
		}

        app->update();
        auto message = app->get_message();
        app->handle_message(message);
    }

	app->exit();

    return 0;
}