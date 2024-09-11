#include "application/application.h"
#include "gpu/gpu.h"
#include "base.h"

std::string app_id = "soft_renderer";
int height = 400;
int width = 800;
int cnt = 0;

void render() {
	gpu->clear();
	cnt %= (height - 1), cnt ++;
	for (int i = 0; i < app->width; i ++) {
		gpu->draw_point(i, cnt, Color(255, 0, 0, 255));
	}
}

int main()
{
    app->init(width, height, app_id);
	gpu->init(app->canvas);

    while (app->active) {
	    render();
        app->update();
        auto message = app->get_message();
        app->handle_message(message);
    }

	app->exit();

    return 0;
}