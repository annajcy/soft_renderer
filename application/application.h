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
    cv::String app_id;
    cv::Mat canvas;
    int width;
    int height;
    bool is_active;

    Application() {}

    ~Application() {
        delete instance;
    }

    void init_application(const std::string &app_id_, int width_, int height_) {

        app_id = app_id_;
        width = width_;
        height = height_;
        is_active = true;

        canvas = cv::Mat::zeros(height, width, CV_8UC3);

        cv::namedWindow(app_id, cv::WINDOW_NORMAL);
        cv::resizeWindow(app_id, width, height);
    }

    static Application* get_instance() {
        if (instance == nullptr) {
            instance = new Application();
        }
        return instance;
    }

    MSG_TYPE get_message() {
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

    void update() {
        cv::imshow(app_id, canvas);
    }
    
};