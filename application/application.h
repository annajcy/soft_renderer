#pragma once

#include "base.h"
#include "../framework/event_center.h"

#define app Application::get_instance()
#define DELTA_TIME 1

class Application
{
private:
    static Application* instance;

    cv::Mat canvas{ };
	std::shared_ptr<u_int8_t[]> canvas_buffer{ nullptr };
	int width{ 0 };
    int height{ 0 };

	static decimal mouse_sensitivity;
	static int mouse_current_x;
	static int mouse_current_y;

    Application() = default;

	static void mouse_callback(int event, int x, int y, int flags, void* userdata) {
		if (event == cv::EVENT_MOUSEMOVE) {
			Event_center<void, std::pair<int, int>, std::pair<decimal, decimal>>::get_instance()->
			trigger_event("on_mouse_move", {x, y},{(x - mouse_current_x) * mouse_sensitivity, (y - mouse_current_y) * mouse_sensitivity});
			mouse_current_x = x, mouse_current_y = y;
		} else if (event == cv::EVENT_LBUTTONDOWN) {
			Event_center<void, std::pair<int, int>, std::pair<decimal, decimal>>::get_instance()->
			trigger_event("on_mouse_left_button_down", {x, y},{(x - mouse_current_x) * mouse_sensitivity, (y - mouse_current_y) * mouse_sensitivity});
			mouse_current_x = x, mouse_current_y = y;
		} else if (event == cv::EVENT_LBUTTONUP) {
			Event_center<void, std::pair<int, int>, std::pair<decimal, decimal>>::get_instance()->
			trigger_event("on_mouse_left_button_up", {x, y},{(x - mouse_current_x) * mouse_sensitivity, (y - mouse_current_y) * mouse_sensitivity});
			mouse_current_x = x, mouse_current_y = y;
		}
	}

	void register_events() {
		Event_center<void>::get_instance()->register_event("quit", [&](){ active = false; });
	}

public:
    cv::String app_id{ "app" };
    bool active{ false };

	void init(int width_, int height_, const std::string &app_id_ , const std::shared_ptr<u_int8_t[]>& frame_buffer) {
		if (!frame_buffer) throw std::invalid_argument("invalid frame buffer");
		canvas_buffer = frame_buffer;
		app_id = app_id_, width = width_, height = height_, active = true;
		canvas = cv::Mat(height, width, CV_8UC3, canvas_buffer.get());
		cv::namedWindow(app_id, cv::WINDOW_NORMAL);
		cv::resizeWindow(app_id, width, height);
		cv::setMouseCallback(app_id, mouse_callback, nullptr);
		register_events();
	}

    static Application* get_instance() {
        if (instance == nullptr) {
            instance = new Application();
        }
        return instance;
    }

	void handle_message() {
		auto key = cv::waitKey(DELTA_TIME);
        if (key == 'q') {
            Event_center<void>::get_instance()->trigger_event("quit");
        } else if (key == 'w') {
	        Event_center<void>::get_instance()->trigger_event("key_w_down");
        } else if (key== 'a') 	{
	        Event_center<void>::get_instance()->trigger_event("key_a_down");
		} else if (key == 's') {
	        Event_center<void>::get_instance()->trigger_event("key_s_down");
		} else if (key == 'd') {
	        Event_center<void>::get_instance()->trigger_event("key_d_down");
		}
	}

    void update() const {
        cv::imshow(app_id, canvas);
    }

    static void exit() {
        cv::destroyAllWindows();
    }
    
};