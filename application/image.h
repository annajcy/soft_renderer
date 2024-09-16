#pragma once

#include "base.h"
#include "color.h"

class Image {
private:
    std::unique_ptr<Color[]> data;

public:
    int width, height;
    Image(int width_, int height_) : width(width_), height(height_), data(std::make_unique<Color[]>(width_ * height_)) { }

    Image(const std::string& path) {
        cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED);

        if (image.empty()) throw std::runtime_error("Error: Unable to load image");

        width = image.cols, height = image.rows;
        data = std::make_unique<Color[]>(width * height);

        auto channels_count = image.channels();

        for (int y = 0; y < height; y ++)
            for (int x = 0; x < width; x ++) {
                Color color;
                if (channels_count == 4) {
                    auto& pixel = image.at<cv::Vec4b>(height - 1 - y, x);
                    color = Color(pixel[2], pixel[1], pixel[0], pixel[3]);
                } else if (channels_count == 3) {
                    auto& pixel = image.at<cv::Vec3b>(height - 1 - y, x);
                    color = Color(pixel[2], pixel[1], pixel[0]);
                } else if (channels_count == 1) {
                    auto& pixel = image.at<uchar>(height - 1 - y, x);
                    color = Color(pixel);
                } else {
                    throw std::runtime_error("Error: Unknown channels count");
                }  
                this->at(x, y) = color; 
            }
    }

    Image(const Image& image) : width(image.width), height(image.height), data(std::make_unique<Color[]>(image.width * image.height)) {
        for (int y = 0; y < height; y ++)
            for (int x = 0; x < width; x ++) {
                this->at(x, y) = image.at(x, y);
            }      
    } 

    Image(Image&& image) : width(image.width), height(image.height), data(std::move(image.data)) { }

    Color& at(int x, int y) {
        if (x < 0 || x >= width) throw std::out_of_range("Error: Out of bound of image");
        if (y < 0 || y >= height) throw std::out_of_range("Error: Out of bound of image");
        return data[y * width + x];
    }

    Color at(int x, int y) const {
        if (x < 0 || x >= width) throw std::out_of_range("Error: Out of bound of image");
        if (y < 0 || y >= height) throw std::out_of_range("Error: Out of bound of image");
        return data[y * width + x];
    }

    Color& at(decimal u, decimal v) {
        int x = std::round(u * (width - 1)), y = std::round(v * (height - 1));
        if (x < 0 || x >= width) throw std::out_of_range("Error: Out of bound of image");
        if (y < 0 || y >= height) throw std::out_of_range("Error: Out of bound of image");
        return data[y * width + x];
    }

    Color at(decimal u, decimal v) const {
        int x = std::round(u * (width - 1)), y = std::round(v * (height - 1));
        if (x < 0 || x >= width) throw std::out_of_range("Error: Out of bound of image");
        if (y < 0 || y >= height) throw std::out_of_range("Error: Out of bound of image");
        return data[y * width + x];
    }

    Color at_bilinear(decimal u, decimal v) const {
        //TODO : at_bilinear
    }

    int size() const {
        return width * height;
    }
    
};