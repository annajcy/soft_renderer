#pragma once

#include "base.h"

namespace math {
	template<typename T, int S>
	class Vec {
		T* data{ nullptr };

	public:
		Vec() {
			data = new T[S];
		}

		explicit Vec(int v) {
			data = new T[S];
			for (int i = 0; i < S; i ++)
				data[i] = v;
		}

		Vec(const Vec& v) {
			data = new T[S];
			for (int i = 0; i < S; i ++)
				data[i] = v[i];
		}

		Vec(Vec&& v)  noexcept {
			data = v.data;
			v.data = nullptr;
		}

		template<int N>
		explicit Vec(const Vec<T, N>& v) {
			data = new T[S];
			for (int i = 0; i < std::min(N, S); i ++)
				data[i] = v[i];
		}

		Vec(const std::initializer_list<T>& list) {
			data = new T[S];
			if (!list.size()) return;
			int cnt = 0;
			for (auto i : list) {
				data[cnt ++] = i;
				if (cnt == S) return;
			}

		}

		~Vec() {
			delete[] data;
		}

		T operator[](int i) const {
			assert(i < S && i >= 0);
			return data[i];
		}

		T& operator[](int i) {
			assert(i < S && i >= 0);
			return data[i];
		}

		int dims() const {
			return S;
		}

		int x() const {
			assert(S >= 1);
			return data[0];
		}

		int& x() {
			assert(S >= 1);
			return data[0];
		}

		int y() const  {
			assert(S >= 2);
			return data[1];
		}

		int& y()  {
			assert(S >= 2);
			return data[1];
		}

		int z() const  {
			assert(S >= 3);
			return data[2];
		}

		int& z()  {
			assert(S >= 3);
			return data[2];
		}

		int w() const {
			assert(S >= 4);
			return data[3];
		}

		int& w() {
			assert(S >= 4);
			return data[3];
		}

		Vec& operator=(const Vec& v) {
			if (this == &v) return *this;
			for (int i = 0; i < S; i ++)
				data[i] = v[i];
			return *this;
		}

		Vec& operator=(Vec&& v)  noexcept {
			if (this == &v) return *this;
			data = v.data;
			v.data = nullptr;
			return *this;
		}

		template<int N>
		Vec& operator=(const Vec<T, N>& v) {
			for (int i = 0; i < std::min(N, S); i ++)
				data[i] = v[i];
			return *this;
		}

		bool operator==(const Vec& v) {
			for (int i = 0; i < S; i ++)
				if (data[i] != v[i]) return false;
			return true;
		}

		Vec operator-() {
			auto res = Vec(*this);
			for (int i = 0; i < S; i ++)
				res[i] = -res[i];
			return res;
		}

		Vec operator+(const Vec& v) {
			auto res = Vec(*this);
			for (int i = 0; i < S; i ++)
				res[i] += v[i];
			return res;
		}

		friend Vec operator+(const Vec& v, const Vec& u) {
			auto res = Vec(v);
			for (int i = 0; i < S; i ++)
				res[i] += u[i];
			return res;
		}

		Vec operator-(const Vec& v) {
			auto res = Vec(*this);
			for (int i = 0; i < S; i ++)
				res[i] -= v[i];
			return res;
		}

		friend Vec dot(const Vec& u, const Vec& v) {
			auto res = Vec(v);
			for (int i = 0; i < S; i ++)
				res[i] *= u[i];
			return res;
		}

		Vec operator*(const T v) {
			auto res = Vec(*this);
			for (int i = 0; i < S; i ++)
				res[i] *= v;
			return res;
		}

		friend Vec operator*(const T v, const Vec& vec) {
			auto res = Vec(vec);
			for (int i = 0; i < S; i ++)
				res[i] *= v;
			return res;
		}

		friend Vec inv_dot(const Vec& u, const Vec& v) {
			auto res = Vec(u);
			for (int i = 0; i < S; i ++) {
				assert(v[i]);
				double inv = 1.0 / v[i];
				res[i] = u[i] * inv;
			}
			return res;
		}

		Vec operator/(const T v) {
			assert(v);
			double inv = 1.0 / v;
			auto res = Vec(*this);
			for (int i = 0; i < S; i ++) {
				res[i] = data[i] * inv;
			}
			return res;
		}

		Vec& operator+=(const Vec& v) {
			for (int i = 0; i < S; i ++)
				data[i] += v[i];
			return *this;
		}

		Vec& operator-=(const Vec& v) {
			for (int i = 0; i < S; i ++)
				data[i] -= v[i];
			return *this;
		}

		Vec& dot(const Vec& v) {
			for (int i = 0; i < S; i ++)
				data[i] *= v[i];
			return *this;
		}

		Vec& operator*=(const T v) {
			for (int i = 0; i < S; i ++)
				data[i] *= v;
			return *this;
		}

		Vec& inv_dot(const Vec& v) {
			for (int i = 0; i < S; i ++) {
				assert(v[i]);
				double inv = 1.0 / v[i];
				data[i] *= inv;
			}
			return *this;
		}

		Vec& operator/=(const T v) {
			assert(v);
			double inv = 1.0 / v;
			for (int i = 0; i < S; i ++) {
				data[i] *= inv;
			}
			return *this;
		}
	};

	using Vec2f = Vec<float, 2>;
	using Vec2i = Vec<int, 2>;
	using Vec3f = Vec<float, 3>;
	using Vec3i = Vec<int, 3>;
	using Vec4f = Vec<float, 4>;
	using Vec4i = Vec<int, 4>;
}