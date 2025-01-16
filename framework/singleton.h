#pragma once

namespace framework{
	template<typename T>
	class Singleton {
	public:
		static T *get_instance() {
			if (instance == nullptr) {
				instance = new T();
			}
			return instance;
		}
		virtual ~Singleton() = default;
	private:
		static T* instance;
		Singleton() = default;
	};

	template<typename T>
	T* Singleton<T>::instance = nullptr;
}

