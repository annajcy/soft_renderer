#include "base.h"
#include "application.h"
#include "gpu.h"
#include "raster.h"
#include "maths.h"
#include "color.h"
#include "image.h"

void test_transformation_2d() {
	auto p1 = math::homo_point(1.0, 0.0);

	auto p2 = math::rotate(45) * p1;
	std::cout << p2 << std::endl;
	auto p3 = math::rotate(-45) * p2;
	std::cout << p3 << std::endl;

	auto p4 = math::translate(1, 2) * p3;
	std::cout << p4 << std::endl;
	auto p5 = math::translate(-1, -2) * p4;
	std::cout << p5 << std::endl;

	auto p6 = math::mirror(math::Vector2d{1.0, 1.0}) * p5;
	std::cout << p6 << std::endl;
	auto p7 = math::mirror(math::Vector2d{1.0, 1.0}) * p6;
	std::cout << p7 << std::endl;

	auto p8 = math::scale(2.0, 2.0) * p7;
	std::cout << p8 << std::endl;
	auto p9 = math::scale(0.5, 0.5) * p8;
	std::cout << p9 << std::endl;
}

void test_transformation_3d() {
    auto p1 = math::homo_point(1.0, 0.0, 0.0);

    auto p2 = math::rotate_zx(45) * p1;
    std::cout << p2 << std::endl;
    auto p3 = math::rotate_zx(-45) * p2;
    std::cout << p3 << std::endl;

    auto p4 = math::translate(1, 2, 3) * p3;
    std::cout << p4 << std::endl;
    auto p5 = math::translate(-1, -2, -3) * p4;
    std::cout << p5 << std::endl;

    auto p6 = math::mirror(math::Vector3d{1.0, 1.0, 0.0}) * p5;
    std::cout << p6 << std::endl;
    auto p7 = math::mirror(math::Vector3d{1.0, 1.0, 0.0}) * p6;
    std::cout << p7 << std::endl;

    auto p8 = math::scale(2.0, 2.0, 2.0) * p7;
    std::cout << p8 << std::endl;
    auto p9 = math::scale(0.5, 0.5, 0.5) * p8;
    std::cout << p9 << std::endl;

	auto p10 = math::rotate({0.0, 1.0, 0.0}, 45) * p9;
    std::cout << p10 << std::endl;
}


int main()
{
	test_transformation_2d();
	test_transformation_3d();

	std::cout << "All tests passed!" << std::endl;
	return 0;
}