#pragma once

#include <opencv2/opencv.hpp>
#include <cstring>

#define app Application::get_instance()
#define DELTA_TIME

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
    unsigned int width{ 0 };
    unsigned int height{ 0 };
    bool is_active{ false };

    Application() = default;

    ~Application() {
        delete canvas;
        delete instance;
    }

    void init(unsigned int width_ = 0, unsigned int height_ = 0, const std::string &app_id_ = "app") {
        app_id = app_id_, width = width_, height = height_, is_active = true;
        canvas = new cv::Mat(height, width, CV_8UC4, cv::Scalar(0, 0, 0, 0));
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
            is_active = false;
        }
    }

    void update() const {
        cv::imshow(app_id, *canvas);
    }
    
};