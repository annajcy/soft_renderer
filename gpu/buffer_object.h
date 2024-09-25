#pragma once

#include "base.h"

template<typename T>
class Buffer_object {
private:
	std::unique_ptr<T[]> data{ nullptr };
	int size_b{0 }, size_d{ 0 };

public:
	Buffer_object() = default;
	~Buffer_object() = default;
	Buffer_object(const Buffer_object& buffer_object) = delete;
	Buffer_object(Buffer_object&& buffer_object) noexcept :
	data(std::move(buffer_object.data)),
	size_b(buffer_object.size_b),
	size_d(buffer_object.size_d) {}


	T* get_data() { return data.get(); }

	void set_buffer_data(T* data_, int size) {
		if (!data_) throw std::invalid_argument("data is invalid");
		if (this->data && this->size_b < size) this->data.reset();

		if (!this->data) {
			this->data = std::make_unique<T[]>(size);
			this->size_b = size;
		}

		this->size_d = size;

		std::memcpy(this->data.get(), data_, size);
	}

	friend std::ostream& operator<<(std::ostream& os, const Buffer_object& buffer_object ) {
		for (int i = 0; i < buffer_object.size_d / sizeof(T); i ++) {
			os << buffer_object.data.get()[i] << ' ';
		}
		return os;
	}

};

using VBO = Buffer_object<decimal>;
using EBO = Buffer_object<int>;

struct VAO {
	int id{ 0 }, stride{ 0 }, offset{ 0 }, item_count{ 0 };

	friend std::ostream& operator<<(std::ostream& os, const VAO& vao ) {
		os << vao.id << ' ' << vao.stride << ' ' << vao.offset << ' ' << vao.item_count << ' ';
		return os;
	}
};

