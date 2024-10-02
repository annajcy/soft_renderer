
#include "event_center.h"

template<>
Event_center<void, std::pair<int, int>, std::pair<double, double>>*
Event_center<void, std::pair<int, int>, std::pair<double, double>>::instance = nullptr;

template<>
Event_center<void>*
Event_center<void>::instance = nullptr;
