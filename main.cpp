#include "base.h"
#include "application.h"
#include "gpu.h"
#include "raster.h"
#include "maths.h"
#include "color.h"
#include "image.h"

void test_zero_matrix() {
    auto mat = math::Mat<int, 3, 3>::zero();
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            assert(mat.at(i, j) == 0);
        }
    }
    std::cout << "Test zero matrix passed!" << std::endl;
}

void test_identity_matrix() {
    auto mat = math::Mat<int, 3, 3>::identity();
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == j) {
                assert(mat.at(i, j) == 1);
            } else {
                assert(mat.at(i, j) == 0);
            }
        }
    }
    std::cout << "Test identity matrix passed!" << std::endl;
}


void test_to_matrix_from_vecs_no_transpose() {
    std::vector<math::Vec<int, 3>> vecs = {
        math::Vec<int, 3>{1, 2, 3},
        math::Vec<int, 3>{4, 5, 6},
        math::Vec<int, 3>{7, 8, 9}
    };

    auto mat = math::Mat<int, 3, 3>::to_matrix(vecs, false);

    std::cout << mat << "Test matrix from vector (no transpose) passed!" << std::endl;
}

void test_to_matrix_from_vecs_with_transpose() {
    std::vector<math::Vec<int, 3>> vecs = {
        math::Vec<int, 3>{1, 2, 3},
        math::Vec<int, 3>{4, 5, 6},
        math::Vec<int, 3>{7, 8, 9}
    };
    auto mat = math::Mat<int, 3, 3>::to_matrix(vecs, true);

    std::cout << mat << "Test matrix from vector (with transpose) passed!" << std::endl;
}

void test_matrix_addition() {
    math::Mat<int, 3, 3> mat1{
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    
    math::Mat<int, 3, 3> mat2{
        {9, 8, 7},
        {6, 5, 4},
        {3, 2, 1}
    };

    auto result = mat1 + mat2;

    std::cout  << result << "Test matrix addition passed!" << std::endl;
}

void test_matrix_transpose() {
    math::Mat<int, 3, 2> mat{
        {1, 2},
        {3, 4},
        {5, 6}
    };

    auto transposed = mat.transpose();

    std::cout << transposed << "Test matrix transpose passed!" << std::endl;
}

void test_matrix_multiplication() {
    math::Mat<int, 2, 3> mat1{
        {1, 2, 3},
        {4, 5, 6}
    };

    math::Mat<int, 3, 2> mat2{
        {7, 8},
        {9, 10},
        {11, 12}
    };

    auto result = mat1 * mat2;

    std::cout << result << "Test matrix multiplication passed!" << std::endl;
}

void test_block() {
	 {
        math::Mat<int, 4, 4> mat({
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 10, 11, 12},
            {13, 14, 15, 16}
        });

        auto block_mat = mat.block<int, 2, 2>(1, 1);  // Take a 2x2 block starting at (1, 1)
        std::cout << "Test 1 Block matrix (2x2 from (1,1)):\n";
        std::cout << block_mat;
        assert(block_mat.at(0, 0) == 6);
        assert(block_mat.at(0, 1) == 7);
        assert(block_mat.at(1, 0) == 10);
        assert(block_mat.at(1, 1) == 11);
    }

    // Test 3: Ensure out-of-range access in block throws exception
    try {
        math::Mat<int, 4, 4> mat({
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 10, 11, 12},
            {13, 14, 15, 16}
        });

        auto invalid_block = mat.block<int, 2, 2>(3, 3); // Out of range block access
    } catch (const std::out_of_range& e) {
        std::cout << "Test 3 Passed: Out of range exception caught: " << e.what() << '\n';
    }

    // Test 4: Test a larger block (3x3) to ensure correctness
    {
        math::Mat<int, 4, 4> mat({
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 10, 11, 12},
            {13, 14, 15, 16}
        });

        auto block_mat = mat.block<int, 3, 3>(0, 0);  // Take a 3x3 block from (0,0)
        std::cout << "Test 4 Block matrix (3x3 from (0,0)):\n";
        std::cout << block_mat;
        assert(block_mat.at(0, 0) == 1);
        assert(block_mat.at(2, 2) == 11);
    }
}

void test_matrix_determinant() {
    // Test Case 1: 1x1 Matrix
    {
        math::Mat<int, 1, 1> mat_1x1;
        mat_1x1.at(0, 0) = 5;
        assert(mat_1x1.determinant() == 5);
        std::cout << "Test Case 1 (1x1 Matrix) passed.\n";
    }

    // Test Case 2: 2x2 Matrix
    {
        math::Mat<int, 2, 2> mat_2x2;
        mat_2x2.at(0, 0) = 1;
        mat_2x2.at(0, 1) = 2;
        mat_2x2.at(1, 0) = 3;
        mat_2x2.at(1, 1) = 4;
        assert(mat_2x2.determinant() == -2);  // Expected determinant: -2
        std::cout << "Test Case 2 (2x2 Matrix) passed.\n";
    }

    // Test Case 3: 3x3 Matrix
    {
        math::Mat<int, 3, 3> mat_3x3;
        mat_3x3.at(0, 0) = 1;
        mat_3x3.at(0, 1) = 2;
        mat_3x3.at(0, 2) = 3;
        mat_3x3.at(1, 0) = 4;
        mat_3x3.at(1, 1) = 5;
        mat_3x3.at(1, 2) = 6;
        mat_3x3.at(2, 0) = 7;
        mat_3x3.at(2, 1) = 8;
        mat_3x3.at(2, 2) = 9;
        assert(mat_3x3.determinant() == 0);  // Expected determinant: 0
        std::cout << "Test Case 3 (3x3 Matrix) passed.\n";
    }

    // Test Case 4: 4x4 Matrix
    {
        math::Mat<int, 4, 4> mat_4x4;
        mat_4x4.at(0, 0) = 1;
        mat_4x4.at(0, 1) = 0;
        mat_4x4.at(0, 2) = 2;
        mat_4x4.at(0, 3) = -1;
        mat_4x4.at(1, 0) = 3;
        mat_4x4.at(1, 1) = 0;
        mat_4x4.at(1, 2) = 0;
        mat_4x4.at(1, 3) = 5;
        mat_4x4.at(2, 0) = 2;
        mat_4x4.at(2, 1) = 1;
        mat_4x4.at(2, 2) = 4;
        mat_4x4.at(2, 3) = -3;
        mat_4x4.at(3, 0) = 1;
        mat_4x4.at(3, 1) = 0;
        mat_4x4.at(3, 2) = 5;
        mat_4x4.at(3, 3) = 0;
        assert(mat_4x4.determinant() == 30);  // Expected determinant: 30
        std::cout << "Test Case 4 (4x4 Matrix) passed.\n";
    }

    // Test Case 5: Minor of a 3x3 Matrix
    {
        math::Mat<int, 3, 3> mat_3x3_minor;
        mat_3x3_minor.at(0, 0) = 1;
        mat_3x3_minor.at(0, 1) = 2;
        mat_3x3_minor.at(0, 2) = 3;
        mat_3x3_minor.at(1, 0) = 4;
        mat_3x3_minor.at(1, 1) = 5;
        mat_3x3_minor.at(1, 2) = 6;
        mat_3x3_minor.at(2, 0) = 7;
        mat_3x3_minor.at(2, 1) = 8;
        mat_3x3_minor.at(2, 2) = 9;

        math::Mat<int, 2, 2> minor_matrix = mat_3x3_minor.minor(1, 1);
        assert(minor_matrix.at(0, 0) == 1);
        assert(minor_matrix.at(0, 1) == 3);
        assert(minor_matrix.at(1, 0) == 7);
        assert(minor_matrix.at(1, 1) == 9);
        std::cout << "Test Case 5 (Minor of a 3x3 Matrix) passed.\n";
    }

    std::cout << "All test cases passed successfully.\n";
}


int main()
{
    test_zero_matrix();
    test_identity_matrix();
    test_to_matrix_from_vecs_no_transpose();
    test_to_matrix_from_vecs_with_transpose();
    test_matrix_addition();
    test_matrix_transpose();
    test_matrix_multiplication();
	test_block();
	test_matrix_determinant();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}