#include "application.h"
#include "gpu.h"
#include "raster.h"
#include "maths.h"
#include "base.h"

const float epsilon = 1e-6f;

void test_default_constructor() {
	math::Vec<float, 6> vec{1, 1, 1, 1, 1};
	for (int i = 0; i < 5; ++i) {
		assert(fabs(vec[i] - 1.0f) < epsilon); // 默认初始化为 0.0f
	}
	std::cout << "test_default_constructor passed\n";
}

void test_value_constructor() {
	math::Vec<float, 3> vec(5.0f);
	for (int i = 0; i < 3; ++i) {
		assert(fabs(vec[i] - 5.0f) < epsilon); // 全部初始化为 5.0f
	}
	std::cout << "test_value_constructor passed\n";
}

void test_copy_constructor() {
	math::Vec<float, 6> vec1(5.0f);
	math::Vec<float, 4> vec2(vec1);
	for (int i = 0; i < 4; ++i) {
		assert(fabs(vec2[i] - 5.0f) < epsilon); // 拷贝构造函数
	}
	std::cout << "test_copy_constructor passed\n";
}

void test_move_constructor() {
	math::Vec<float, 3> vec1(5.0f);
	math::Vec<float, 3> vec2(std::move(vec1));
	for (int i = 0; i < 3; ++i) {
		assert(fabs(vec2[i] - 5.0f) < epsilon); // 移动构造函数
	}
	std::cout << "test_move_constructor passed\n";
}

void test_assignment_operator() {
	math::Vec<float, 3> vec1(5.0f);
	math::Vec<float, 3> vec2;
	vec2 = vec1; // 赋值操作符
	for (int i = 0; i < 3; ++i) {
		assert(fabs(vec2[i] - 5.0f) < epsilon);
	}
	std::cout << "test_assignment_operator passed\n";
}

void test_move_assignment_operator() {
	math::Vec<float, 3> vec1(5.0f);
	math::Vec<float, 3> vec2;
	vec2 = std::move(vec1); // 移动赋值操作符
	for (int i = 0; i < 3; ++i) {
		assert(fabs(vec2[i] - 5.0f) < epsilon);
	}
	std::cout << "test_move_assignment_operator passed\n";
}

void test_arithmetic_operations() {
	math::Vec<float, 3> vec1(2.0f);
	math::Vec<float, 3> vec2(3.0f);

	auto vec3 = -vec1;
	for (int i = 0; i < 3; i ++) {
		assert(fabs(vec3[i] + 2.0f) < epsilon);
	}

	vec3 = vec1 + vec2; // 加法
	for (int i = 0; i < 3; ++i) {
		assert(fabs(vec3[i] - 5.0f) < epsilon);
	}

	vec3 = vec1 - vec2; // 减法
	for (int i = 0; i < 3; ++i) {
		assert(fabs(vec3[i] + 1.0f) < epsilon); // 结果应该是 -1.0f
	}

	vec3 = vec1 * 2.0f; // 标量乘法
	for (int i = 0; i < 3; ++i) {
		assert(fabs(vec3[i] - 4.0f) < epsilon);
	}

	vec3 = vec1 / 2.0f; // 标量除法
	for (int i = 0; i < 3; ++i) {
		assert(fabs(vec3[i] - 1.0f) < epsilon);
	}

	std::cout << "test_arithmetic_operations passed\n";
}

void test_dot_product() {
	math::Vec<float, 3> vec1(2.0f);
	math::Vec<float, 3> vec2(3.0f);

	auto vec3 = dot(vec1, vec2); // 点乘
	for (int i = 0; i < 3; ++i) {
		assert(fabs(vec3[i] - 6.0f) < epsilon);
	}

	std::cout << "test_dot_product passed\n";
}

// 新增自乘、自加、自减运算符的测试
void test_compound_assignment_operators() {
	math::Vec<float, 3> vec1(2.0f);
	math::Vec<float, 3> vec2(3.0f);

	vec1 += vec2; // 自加运算符
	for (int i = 0; i < 3; ++i) {
		assert(fabs(vec1[i] - 5.0f) < epsilon);
	}

	vec1 -= vec2; // 自减运算符
	for (int i = 0; i < 3; ++i) {
		assert(fabs(vec1[i] - 2.0f) < epsilon); // 回到原来的值
	}

	vec1 *= 2.0f; // 自乘运算符
	for (int i = 0; i < 3; ++i) {
		assert(fabs(vec1[i] - 4.0f) < epsilon); // 每个元素应乘以 2
	}

	vec1 /= 2.0f; // 自除运算符
	for (int i = 0; i < 3; ++i) {
		assert(fabs(vec1[i] - 2.0f) < epsilon); // 每个元素应除以 2，回到原来的值
	}

	std::cout << "test_compound_assignment_operators passed\n";
}

int main() {
	test_default_constructor();
	test_value_constructor();
	test_copy_constructor();
	test_move_constructor();
	test_assignment_operator();
	test_move_assignment_operator();
	test_arithmetic_operations();
	test_dot_product();
	test_compound_assignment_operators(); // 测试复合运算符

	std::cout << "All tests passed!\n";
	return 0;
}
