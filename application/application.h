#pragma once

#include "base.h"

#define app Application::get_instance()
#define DELTA_TIME 0

enum MSG_TYPE {
    EXIT,
    OTHER,
};

class Application
{
private:
    static Application* instance;

public:
    cv::String app_id{ "app" };
    cv::Mat* canvas{ nullptr };
    int width{ 0 };
    int height{ 0 };
    bool active{ false };

    Application() = default;

    ~Application() {
        delete canvas;
        delete instance;
    }

    void init(int width_ = 0, int height_ = 0, const std::string &app_id_ = "app") {
        app_id = app_id_, width = width_, height = height_, active = true;
        canvas = new cv::Mat(height, width, CV_8UC3, cv::Scalar(0, 0, 0));
        cv::namedWindow(app_id, cv::WINDOW_NORMAL);
        cv::resizeWindow(app_id, width, height);
    }

    static Application* get_instance() {
        if (instance == nullptr) {
            instance = new Application();
        }
        return instance;
    }

    static MSG_TYPE get_message() {
        if (cv::waitKey(DELTA_TIME) == 'q') {
            return EXIT;
        } else {
            return OTHER;
        }
    }

    void handle_message(MSG_TYPE message) {
        if (message == EXIT) {
            active = false;
        }
    }

    void update() const {
        cv::imshow(app_id, *canvas);
    }

    void exit() {
        cv::destroyAllWindows();
    }
    
};