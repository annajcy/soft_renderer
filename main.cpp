#include <opencv2/opencv.hpp>
#include "application/application.h"
#include "base/base.h"

std::string app_id = "soft_renderer";
int height = 800;
int width = 1600;

int main()
{
    app->init_application(app_id, width, height);

    while (app->is_active) {
        app->update();
        auto message = app->get_message();
        app->handle_message(message);
    }

    return 0;
}