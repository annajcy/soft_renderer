#pragma once

#include "base.h"
#include "vec.h"

namespace math {
    template <Arithmetic T, int S, int V>
    class Mat {
        static_assert(S > 0 && V > 0, "Matrix dimensions must be positive");
    private:
        std::array<T, S * V> data{};

    public:
        static Mat<T, S, V> zero() { return Mat<T, S, V>(0); }

        static Mat<T, S, V> identity() requires Equal<int, S, V> {
            Mat<T, S, V> mat;
            for (int i = 0; i < S; i ++)
                mat.at(i, i) = 1;
            return mat; 
        }

        template<typename N, int P> 
        static Mat<T, S, V>to_matrix(const std::vector<math::Vec<N, P>>& vec_s, bool transpose = false) {
            Mat<T, S, V> mat;
            if (transpose) {
                for (int i = 0; i < std::min(S, (int)vec_s.size()); i ++) {
                    for (int j = 0; j < std::min(V, P); j ++)
                        mat.at(i, j) = vec_s[i][j];
                }
            } else {
                for (int j = 0; j < std::min(V, (int)vec_s.size()); j ++) {
                    for (int i = 0; i < std::min(S, P); i ++)
                        mat.at(i, j) = vec_s[j][i];
                }
            }
            return mat;
        }

        template<typename N, int P> 
        static Mat<T, S, V>to_matrix(math::Vec<N, P>& vec_s, bool transpose = false) {
            Mat<T, S, V> mat;
            if (transpose) {
                for (int i = 0; i < S; i ++) {
                    for (int j = 0; j < std::min(V, P); j ++)
                        mat.at(i, j) = vec_s[j];
                }
            } else {
                for (int j = 0; j < V; j ++) {
                    for (int i = 0; i < std::min(S, P); i ++)
                        mat.at(i, j) = vec_s[i];
                }
            }
            return mat;
        }

        Mat<T, S, V>() : data{} { }
        explicit Mat<T, S, V>(int value) { data.fill(value); }
        Mat<T, S, V>(const Mat<T, S, V>& mat) : data(mat.data) { }
        Mat<T, S, V>(Mat<T, S, V>&& mat) noexcept : data(std::move(mat.data)) { }

        template<typename N, int P, int Q>
        explicit Mat<T, S, V>(const Mat<N, P, Q>& mat) {
            for (int i = 0; i < std::min(P, S); i ++)
                for (int j = 0; j < std::min(V, Q); j ++)
                    this->at(i, j) = mat.at(i, j);
        }

        template<typename N, int P> 
        explicit Mat<T, S, V>(const std::vector<math::Vec<N, P>>& vec_s, bool transpose = false) {
            if (transpose) {
                for (int i = 0; i < std::min(S, vec_s.size()); i ++) {
                    for (int j = 0; j < std::min(V, P); j ++)
                        this->at(i, j) = vec_s[i][j];
                }
            } else {
                for (int j = 0; j < std::min(V, vec_s.size()); j ++) {
                    for (int i = 0; i < std::min(S, P); j ++)
                        this->at(i, j) = vec_s[j][i];
                }
            }
        }

        template<typename N, int P> 
        explicit Mat<T, S, V>(math::Vec<N, P>& vec_s, bool transpose = false) {
            if (transpose) {
                for (int i = 0; i < S; i ++) {
                    for (int j = 0; j < std::min(V, P); j ++)
                        this->at(i, j) = vec_s[j];
                }
            } else {
                for (int j = 0; j < V; j ++) {
                    for (int i = 0; i < std::min(S, P); j ++)
                        this->at(i, j) = vec_s[i];
                }
            }
        }

        Mat<T, S, V>(const std::initializer_list<std::initializer_list<varied_type>>& lists) {
            int i = 0, j = 0;
            for (const auto &vec : lists) {
                j = 0;
                for (const auto &v : vec) {
                    std::visit([&](auto&& arg) {
                        this->at(i, j) = static_cast<T>(arg);
                    }, v);
                    if (++ j == V) break;
                }
                if (++ i == S) return;
            }
        }

        [[nodiscard]] std::pair<int, int> shape() const { return {S, V}; }
        
        T& at(int i, int j) {
            if (i < 0 || i >= S) throw std::out_of_range("Error: Out of range.");
            if (j < 0 || j >= V) throw std::out_of_range("Error: Out of range.");
            return data[i * V + j];
        }

        T at(int i, int j) const {
            if (i < 0 || i >= S) throw std::out_of_range("Error: Out of range.");
            if (j < 0 || j >= V) throw std::out_of_range("Error: Out of range.");
            return data[i * V + j];
        }

        Mat<T, S, V>& operator=(const Mat<T, S, V>& rhs) {
            if (this == &rhs) return *this;
            this->data = rhs.data;
            return *this;
        }

        Mat<T, S, V>& operator=(Mat<T, S, V>&& rhs) noexcept {
            if (this == &rhs) return *this;
            this->data = std::move(rhs.data);
            return *this;
        }

        template<typename N, int P, int Q>
        Mat<T, S, V>& operator=(const Mat<N, P, Q>& mat) {
            for (int i = 0; i < std::min(P, S); i ++)
                for (int j = 0; j < std::min(V, Q); j ++)
                    this->at(i, j) = mat.at(i, j);
            return *this;
        }

        Mat<T, S, V> operator-() const {
            Mat<T, S, V> mat;
            for (int i = 0; i < S; i ++) 
                for (int j = 0; j < V; j ++)
                    mat[i] = -this->at(i, j);
            return mat;
        }

        template<typename N, int P, int Q>
        Mat<T, S, V> operator-(const Mat<N, P, Q>& rhs) const {
            Mat<T, S, V> mat;
            for (int i = 0; i < std::min(P, S); i ++)
                for (int j = 0; j < std::min(V, Q); j ++)
                   mat.at(i, j) = this->at(i, j) - rhs.at(i, j);
            return mat;
        }

        template<typename N, int P, int Q>
        Mat<T, S, V> operator+(const Mat<N, P, Q>& rhs) const {
            Mat<T, S, V> mat;
            for (int i = 0; i < std::min(P, S); i ++)
                for (int j = 0; j < std::min(V, Q); j ++)
                   mat.at(i, j) = this->at(i, j) + rhs.at(i, j);
            return mat;
        }

        template<typename N, int Q>
        Mat<T, S, Q> operator*(const Mat<N, V, Q>& rhs) const {
            Mat<T, S, Q> mat;
            for (int i = 0; i < S; i ++)
                for (int j = 0; j < Q; j ++)
                    for (int k = 0; k < V; k ++)
                        mat.at(i, j) += this->at(i, k) * rhs.at(k, j);
            return mat;
        }

        template<typename N>
        Vec<T, S> operator*(const Vec<N, V>& rhs) const {
            Vec<T, S> vec;
            for (int i = 0; i < S; i ++)
                for (int k = 0; k < V; k ++)
                    vec[i] += this->at(i, k) * rhs[k];
            return vec;
        }

        template<typename N, int P, int Q>
        Mat<T, S, V>& operator-=(const Mat<N, P, Q>& rhs) {
            for (int i = 0; i < std::min(P, S); i ++)
                for (int j = 0; j < std::min(V, Q); j ++)
                   this->at(i, j) -= rhs.at(i, j);
        }

        template<typename N, int P, int Q>
        Mat<T, S, V>& operator+=(const Mat<N, P, Q>& rhs) {
            for (int i = 0; i < std::min(P, S); i ++)
                for (int j = 0; j < std::min(V, Q); j ++)
                   this->at(i, j) += rhs.at(i, j);
            return *this;
        }

        template<typename N>
        Mat<T, S, V>& operator*=(const Mat<N, S, V>& rhs) requires Equal<int, S, V> {
            Mat<T, S, V> temp = *this * rhs;
            *this = std::move(temp);
            return *this;
        }

        Mat<T, V, S> transpose() const {
            Mat<T, V, S> mat;
            for (int i = 0; i < V; i ++)
                for (int j = 0; j < S; j ++)
                    mat.at(i, j) = this->at(j, i);
            return mat;
        }

        template<typename, int P, int Q>
        Mat<T, P, Q> block(int st_i, int st_j) const {
            Mat<T, P, Q> mat;
            for (int i = 0; i < P; i ++ )
                for (int j = 0; j < Q; j ++) {
                    mat.at(i, j) = this->at(i + st_i, j + st_j);
                }
            return mat;
        }

        Mat<T, S - 1, V - 1> minor(int x, int y) const requires (S > 1 && V > 1 && Equal<int, S, V>) {
            Mat<T, S - 1, V - 1> mat;
            for (int i = 0; i < S - 1; i ++)
                for (int j = 0; j < V - 1; j ++) 
                    mat.at(i, j) = this->at((i < x ? i : i + 1), (j < y ? j : j + 1));
            return mat;
        }

        T det() const requires Equal<int, S, V> {
            if constexpr (S == 1) return this->at(0, 0);
            else {
                T result = 0;
                for (int i = 0; i < S; i ++)
                    result += ((i & 1) ? -1 : 1) * this->at(i, 0) * this->minor(i, 0).det();
                return result;
            }
        }

        Mat<T, S, V> inv() const requires Equal<int, S, V> {
            auto adjugate = this->adjugate();
            T determinant = this->det();
            if (!determinant) throw std::runtime_error("Matrix is singular and cannot be inverted.");
            return adjugate / determinant;
        }

        Mat<T, S, V> adjugate() const {
            Mat<T, S, V> mat;
            for (int i = 0; i < S; i++) 
                for (int j = 0; j < V; j++) {
                    T cofactor = this->minor(i, j).det();
                    mat.at(j, i) = (((i + j) & 1) ? -1 : 1) * cofactor;
                }
            return mat;
        }

        template<typename N>
        Mat<T, S, V>& operator/=(const N& scalar) {
            for (int i = 0; i < S; i++)
                for (int j = 0; j < V; j++)
                    this->at(i, j) /= scalar;
            return *this;
        }

        template<typename N>
        Mat<T, S, V> operator/(const N& scalar) const {
            Mat<T, S, V> mat;
            for (int i = 0; i < S; i++)
                for (int j = 0; j < V; j++)
                    mat.at(i, j) = this->at(i, j) / scalar;
            return mat;
        }

        friend std::ostream& operator<<(std::ostream& os, const Mat<T, S, V>& mat) {
            for (int i = 0; i < S; i ++) {
                for (int j = 0; j < V; j ++)
	                os << mat.at(i, j) << ' ';
	            os << std::endl;
            }
            return os;
        }

    };

    using Mat2x2 = Mat<decimal, 2, 2>;
    using Mat3x3 = Mat<decimal, 3, 3>;
    using Mat4x4 = Mat<decimal, 4, 4>;

    using Mat2x2i = Mat<int, 2, 2>;
    using Mat3x3i = Mat<int, 3, 3>;
    using Mat4x4i = Mat<int, 4, 4>;
}
