#pragma once

#include "base.h"

struct VAO {
	int id{ 0 }, stride{ 0 }, offset{ 0 }, item_size{ 0 };

	friend std::ostream& operator<<(std::ostream& os, const VAO& vao ) {
		os << vao.id << ' ' << vao.stride << ' ' << vao.offset << ' ' << vao.item_size << ' ';
		return os;
	}
};

template<typename T>
class Buffer_object {
private:
	std::unique_ptr<T[]> data{ nullptr };
	

public:
	Buffer_object() = default;
	~Buffer_object() = default;
	Buffer_object(const Buffer_object& buffer_object) = delete;
	Buffer_object(Buffer_object&& buffer_object) noexcept :
			data(std::move(buffer_object.data)),
			size_buffer(buffer_object.size_buffer),
			size_data(buffer_object.size_data) {}

	int size_buffer{ 0 }, size_data{ 0 };

	std::unique_ptr<T[]> get_buffer_data(int index, int stride, int offset, int item_size) {
		int pointer = index * stride + offset;
		std::unique_ptr<T[]> result(std::make_unique<T[]>(item_size));
		std::memcpy(result.get(), this->data.get() + pointer, item_size * sizeof(T));
		return result;
	}

	void set_buffer_data(const void* data_, int size_data_) {
		if (!data_) throw std::invalid_argument("data is invalid");
		if (this->data && this->size_buffer < size_data_) this->data.reset();

		if (!this->data) {
			this->data = std::make_unique<T[]>(size_data_);
			this->size_buffer = size_data_;
		}

		this->size_data = size_data_;
		std::memcpy(this->data.get(), data_, size_data_ * sizeof(T));

	}

	friend std::ostream& operator<<(std::ostream& os, const Buffer_object& buffer_object ) {
		for (int i = 0; i < buffer_object.size_data; i ++) {
			os << buffer_object.data[i] << ' ';
		}
		return os;
	}

};

using VBO = Buffer_object<decimal>;
using EBO = Buffer_object<int>;



