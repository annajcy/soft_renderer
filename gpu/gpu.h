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

    //the color format of opencv is BGR
    void set_pixel(int x, int y, const math::Color& color, bool blend = true) {
        if (x < 0 || x >= width) return;
        if (y < 0 || y >= height) return;
        auto& pixel = canvas->at<cv::Vec3b>(height - 1 - y, x);
        if (blend) {
	        math::Color background_color(pixel[2], pixel[1], pixel[0]);
            auto [r, g, b, _] = math::Color::alpha_blend(color, background_color);
            pixel = cv::Vec3b(b, g, r);
        } else {
            auto [r, g, b, _] = color;
            pixel = cv::Vec3b(b, g, r);
        }
    }

};