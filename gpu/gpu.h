#pragma once

#include "base.h"
#include "color.h"

#define gpu GPU::get_instance()

class GPU
{
private:

    static GPU* instance;
    int width;
    int height;
    cv::Mat* canvas;

public:

    GPU() = default;

    ~GPU() {
        delete instance;    
    }

    static GPU* get_instance() {
        if (instance == nullptr) {
            instance = new GPU();
        }
        return instance;
    }

    void init(cv::Mat* canvas_) {
        canvas = canvas_, width = canvas->cols, height = canvas->rows;
    }

    void clear() {
        canvas->setTo(cv::Scalar(0, 0, 0));
    }

    void set_pixel(int x, int y, const Color& color) {
        auto& pixel = canvas->at<cv::Vec3b>(height - 1 - y, x);
        float alpha_factor = color.A() / 255.0f;
        cv::Vec3b blended_pixel(
            pixel[0] * (1 - alpha_factor) + color.B() * alpha_factor,
            pixel[1] * (1 - alpha_factor) + color.G() * alpha_factor,
            pixel[2] * (1 - alpha_factor) + color.R() * alpha_factor
        );
        pixel = blended_pixel;
    }

};