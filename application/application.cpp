
#include "application.h"

Application* Application::instance = nullptr;

decimal Application::mouse_sensitivity{ 0.1 };
int Application::mouse_current_x{};
int Application::mouse_current_y{};