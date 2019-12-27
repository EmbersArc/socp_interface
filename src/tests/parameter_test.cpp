#include "parameter.hpp"

#include <iostream>
#include <cassert>

#ifdef EIGEN_AVAILABLE
#include <Eigen/Dense>
#endif

int main()
{
    // Scalar
    op::Parameter scalar_param_const(2.0);
    double scalar = 3.;
    op::Parameter scalar_param_ptr(&scalar);

    op::Parameter result_scalar = scalar_param_ptr + scalar_param_const + scalar_param_ptr + scalar_param_const;
    assert(result_scalar.getValue() == 10.);

    result_scalar = scalar_param_ptr + scalar_param_const - scalar_param_ptr - scalar_param_const;
    assert(result_scalar.getValue() == 0.);

    result_scalar = scalar_param_ptr * scalar_param_const - scalar_param_ptr * scalar_param_const;
    assert(result_scalar.getValue() == 0.);

    result_scalar = scalar_param_ptr * scalar_param_const * scalar_param_ptr * scalar_param_const;
    assert(result_scalar.getValue() == 36.);

    scalar = 1.;
    result_scalar = scalar_param_const + scalar_param_ptr;
    assert(result_scalar.getValue() == 3.);
    result_scalar = scalar_param_const - scalar_param_ptr;
    assert(result_scalar.getValue() == 1.);
    result_scalar = scalar_param_const * scalar_param_ptr;
    assert(result_scalar.getValue() == 2.);
    result_scalar = scalar_param_const / scalar_param_ptr;
    assert(result_scalar.getValue() == 2.);

    // Matrix
    // multiply 2x2/2x2
    op::parameter_source_matrix_t matrix = {{1., 2.},
                                            {3., 4.}};
    op::Parameter matrix_parameter(matrix);
    op::Parameter result_matrix = matrix_parameter * matrix_parameter;
    assert(result_matrix.getValues() == op::double_matrix_t({{7., 10.}, {15., 22.}}));

    // multiply 2x1/2x2
    op::parameter_source_matrix_t vector = {{1., 2.}};
    op::Parameter vector_parameter(vector);
    result_matrix = vector_parameter * matrix_parameter;
    assert(result_matrix.getValues() == op::double_matrix_t({{7., 10.}}));

    // multiply 1x1/2x2
    result_matrix = scalar_param_const * matrix_parameter;
    assert(result_matrix.getValues() == op::double_matrix_t({{2., 4.}, {6., 8.}}));

    // multiply 2x2/1x1
    result_matrix = matrix_parameter * scalar_param_const;
    assert(result_matrix.getValues() == op::double_matrix_t({{2., 4.}, {6., 8.}}));

#ifdef EIGEN_AVAILABLE
    Eigen::Matrix3d m1, m2, m3;
    m1.setRandom();
    m2.setRandom();
    m3.setRandom();
    op::Parameter eigen1(m1);
    op::Parameter eigen2(m2);
    op::Parameter eigen3(&m3);

    Eigen::Matrix3d m = m1 * m2 * m3;
    op::Parameter result = eigen1 * eigen2 * eigen3;

    for (size_t row = 0; row < size_t(m.rows()); row++)
    {
        for (size_t col = 0; col < size_t(m.cols()); col++)
        {
            assert(result.getValue(row, col) == m(row, col));
        }
    }

    m3.setRandom();
    m = m1 * m2 * m3;

    for (size_t row = 0; row < size_t(m.rows()); row++)
    {
        for (size_t col = 0; col < size_t(m.cols()); col++)
        {
            assert(result.getValue(row, col) == m(row, col));
        }
    }
#endif

    std::cout << "All tests were successful."
              << "\n";
}