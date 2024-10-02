#pragma once

#include "base.h"

template<typename T, typename ...Args>
class Event_center {
private:
	static Event_center* instance;
	Event_center() = default;
	std::unordered_map<std::string, std::function<T(Args...)>> events;
public:
	static Event_center* get_instance() {
		if (!instance) {
			instance = new Event_center();
		}
		return instance;
	}

	void register_event(const std::string& event_name, std::function<T(Args...)> handler) {
		events[event_name] = handler;
	}

	T trigger_event(const std::string& event_name, Args... args) {
		if (events.contains(event_name)) {
			return events[event_name](args...);
		} else {
			throw std::invalid_argument("Unregistered event accessed");
		}
	}
};
