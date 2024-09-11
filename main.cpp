#include <opencv2/opencv.hpp>
#include "application/application.h"
#include "gpu/gpu.h"
#include "base/base.h"

std::string app_id = "soft_renderer";
int height = 800;
int width = 1600;

void render() {
	gpu->clear();
	for (int i = 0; i < app->width; i ++) {
		for (int j = 0; j < app->height; j ++) {
			gpu->draw_point(i, j, Color(100, 50, 50, 255));
		}
	}
}

int main()
{
    app->init(width, height, app_id);
	gpu->init(width, height, app->canvas);

    while (app->is_active) {
	    render();
        app->update();
        auto message = app->get_message();
        app->handle_message(message);
    }

    return 0;
}