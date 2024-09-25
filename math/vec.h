#pragma once

#include "base.h"
#include <array>

namespace math {

	template<Arithmetic T, int S>
	class Vec {
		static_assert(S > 0, "Vector dimensions must be positive");
	private:
		std::array<T, S> data{};

	public:

		static Vec<T, S> zeros() { return Vec<T, S>(0); }
		static Vec<T, S> ones() { return Vec<T, S>(1); }

		Vec<T, S>() : data{} { }
		explicit Vec<T, S>(int value) { data.fill(value); }
		Vec<T, S>(const Vec<T, S>& vec) : data(vec.data) { }
		Vec<T, S>(Vec<T, S>&& vec) noexcept : data(std::move(vec.data)) { }

		template<typename Q, int N>
		explicit Vec<T, S>(const Vec<Q, N>& vec) {
			for (int i = 0; i < std::min(N, S); i++)
				data[i] = vec[i];
		}

		Vec<T, S>(const std::initializer_list<varied_type>& list) {
			int x = 0;
			for (const auto &i : list) {
				std::visit([&](auto&& arg) {
					data[x] = static_cast<T>(arg);
				}, i);
				if (++ x == S) return;
			}
		}

		Vec<T, S>& operator=(const Vec<T, S>& rhs) {
			if (this == &rhs) return *this;
			data = rhs.data;
			return *this;
		}

		template<typename Q, int N>
		Vec<T, S>& operator=(const Vec<Q, N>& rhs) {
			for (int i = 0; i < std::min(N, S); i++)
				data[i] = rhs[i];
			return *this;
		}

		Vec<T, S>& operator=(Vec<T, S>&& rhs) noexcept {
			if (this == &rhs) return *this;
			data = std::move(rhs.data);
			return *this;
		}

		T& x() requires Greater_than<int, S, 1> { return data[0]; }
		T& y() requires Greater_than<int, S, 2> { return data[1]; }
		T& z() requires Greater_than<int, S, 3> { return data[2]; }
		T& w() requires Greater_than<int, S, 4> { return data[3]; }

		T x() const requires Greater_than<int, S, 1> { return data[0]; }
		T y() const requires Greater_than<int, S, 2> { return data[1]; }
		T z() const requires Greater_than<int, S, 3> { return data[2]; }
		T w() const requires Greater_than<int, S, 4> { return data[3]; }

		[[nodiscard]] int dims() const { return S; }

		T operator[](int index) const {
			if (index < 0 || index >= S) throw std::out_of_range("Index out of range");
			return data[index];
		}

		T& operator[](int index) {
			if (index < 0 || index >= S) throw std::out_of_range("Index out of range");
			return data[index];
		}

		bool operator==(const Vec<T, S>& vec) const {
			return data == vec.data;
		}

		bool operator==(const Vec<T, S>& vec) const requires Floating_point<T> {
			for (int i = 0; i < S; i++)
				if (!equal(this->data[i], vec[i])) return false;
			return true;
		}

		Vec<T, S> operator-() const {
			Vec<T, S> result;
			for (int i = 0; i < S; i++)
				result[i] = -data[i];
			return result;
		}

		template<typename N>
		Vec<T, S> operator+(const Vec<N, S>& rhs) const {
			Vec<T, S> result;
			for (int i = 0; i < S; i++)
				result[i] = data[i] + rhs[i];
			return result;
		}

		template<typename N>
		Vec<T, S> operator-(const Vec<N, S>& rhs) const {
			Vec<T, S> result;
			for (int i = 0; i < S; i++)
				result[i] = data[i] - rhs[i];
			return result;
		}

		template<typename N>
		Vec<T, S> operator*(const N& scalar) const {
			Vec<T, S> result;
			for (int i = 0; i < S; i++)
				result[i] = data[i] * scalar;
			return result;
		}

		template<typename Q>
		Vec<T, S> operator*(const Vec<Q, S>& rhs) const {
			Vec<T, S> result;
			for (int i = 0; i < S; i++)
				result[i] = data[i] * rhs[i];
			return result;
		}

		template<typename N>
		Vec<T, S> operator/(const N& rhs) const {
			if (!rhs) throw std::runtime_error("Divided by zero");
			Vec<T, S> result;
			for (int i = 0; i < S; i++)
				result[i] = data[i] / rhs;
			return result;
		}

		template<typename N>
		Vec<T, S>& operator+=(const Vec<N, S>& rhs) {
			for (int i = 0; i < S; i++)
				data[i] += rhs[i];
			return *this;
		}

		template<typename N>
		Vec<T, S>& operator-=(const Vec<N, S>& rhs) {
			for (int i = 0; i < S; i++)
				data[i] -= rhs[i];
			return *this;
		}

		template<typename N>
		Vec<T, S>& operator*=(const N& rhs) {
			for (int i = 0; i < S; i++)
				data[i] *= rhs;
			return *this;
		}

		template<class N>
		Vec<T, S>& operator/=(const N& rhs) {
			if (!rhs) throw std::runtime_error("Divided by zero");
			for (int i = 0; i < S; i++)
				data[i] /= rhs;
			return *this;
		}

		T dot(const Vec<T, S>& v) const {
			T result = 0;
			for (int i = 0; i < S; i++)
				result += data[i] * v[i];
			return result;
		}

		[[nodiscard]] decimal norm() const {
			decimal result = 0;
			for (int i = 0; i < S; i++)
				result += data[i] * data[i];
			return std::sqrt(result);
		}

		Vec<decimal, S> normalize() const {
			decimal norm = this->norm();
			Vec<decimal, S> result;
			for (int i = 0; i < S; i++)
				result[i] = data[i] / norm;
			return result;
		}

		Vec<decimal, S> project_to(const Vec<T, S>& vec) const {
			Vec<decimal, S> result;
			auto vec_norm = vec.norm();
			decimal factor = this->dot(vec) / (vec_norm * vec_norm);
			for (int i = 0; i < S; i++)
				result[i] = vec[i] * factor;
			return result;
		}

		friend std::ostream& operator<<(std::ostream& os, const Vec<T, S>& vec) {
			for (int i = 0; i < S; i++)
				os << vec.data[i] << ' ';
			return os;
		}

		friend std::ostream& operator<<(std::ostream& os, const Vec<T, S>& vec) requires Floating_point<T> {
			for (int i = 0; i < S; i++)
				os << (!sign(vec.data[i]) ? (decimal)0 : vec.data[i]) << ' ';
			os << std::endl;
			return os;
		}

		template<typename N, typename P>
		friend Vec<T, S> operator*(const N& lhs, const Vec<P, S>& rhs) {
			Vec<T, S> result;
			for (int i = 0; i < S; i++)
				result[i] = lhs * rhs[i];
			return result;
		}

		template<typename Q, int N>
		friend Vec<Q, N> cast(const Vec<T, S>& vec) {
			Vec<Q, N> result;
			for (int i = 0; i < std::min(N, S); i++)
				result[i] = vec[i];
			return result;
		}

		template<typename Q>
		friend Vec<Q, S> cast_type(const Vec<T, S>& vec) {
			Vec<Q, S> result;
			for (int i = 0; i < S; i++)
				result[i] = vec[i];
			return result;
		}

		template<int N>
		friend Vec<T, N> cast_dims(const Vec<T, S>& vec) {
			Vec<T, N> result;
			for (int i = 0; i < std::min(N, S); i++)
				result[i] = vec[i];
			return result;
		}
	};

	using Vec2f = Vec<decimal, 2>;
	using Vec2i = Vec<int, 2>;
	using Vec3f = Vec<decimal, 3>;
	using Vec3i = Vec<int, 3>;
	using Vec4f = Vec<decimal, 4>;
	using Vec4i = Vec<int, 4>;

}
