#include "parameter.hpp"

#include <iostream>
#include <cassert>

// #ifdef EIGEN_AVAILABLE
#include <Eigen/Dense>

template <typename Derived>
bool compareMatrices(op::Parameter &param, Eigen::DenseBase<Derived> &matrix)
{
    bool is_same = true;
    for (size_t row = 0; row < size_t(matrix.rows()); row++)
    {
        for (size_t col = 0; col < size_t(matrix.cols()); col++)
        {
            is_same &= std::abs(param.getValue(row, col) - matrix(row, col)) < 1e-8;
        }
    }
    return is_same;
}

// #endif

int main()
{
    // Scalar
    op::Parameter scalar_param_1(1.0);
    op::Parameter scalar_param_2(2.0);
    op::Parameter scalar_param_3(3.0);
    op::Parameter scalar_param_4(4.0);
    double scalar = 3.;
    op::Parameter scalar_param_ptr(&scalar);

    op::Parameter result_scalar = scalar_param_ptr + scalar_param_2 + scalar_param_ptr + scalar_param_2;
    assert(result_scalar.getValue() == 10.);

    result_scalar = scalar_param_ptr + scalar_param_2 - scalar_param_ptr - scalar_param_2;
    assert(result_scalar.getValue() == 0.);

    result_scalar = scalar_param_ptr * scalar_param_2 - scalar_param_ptr * scalar_param_2;
    assert(result_scalar.getValue() == 0.);

    result_scalar = scalar_param_ptr * scalar_param_2 * scalar_param_ptr * scalar_param_2;
    assert(result_scalar.getValue() == 36.);

    scalar = 1.;
    result_scalar = scalar_param_2 + scalar_param_ptr;
    assert(result_scalar.getValue() == 3.);
    result_scalar = scalar_param_2 - scalar_param_ptr;
    assert(result_scalar.getValue() == 1.);
    result_scalar = scalar_param_2 * scalar_param_ptr;
    assert(result_scalar.getValue() == 2.);
    result_scalar = scalar_param_2 / scalar_param_ptr;
    assert(result_scalar.getValue() == 2.);

    // Matrix
    // multiply 2x2/2x2
    scalar = 3.;
    op::parameter_source_matrix_t matrix = {{1., 2.},
                                            {op::ParameterSource(&scalar), 4.}};
    op::Parameter matrix_parameter(matrix);
    op::Parameter result_matrix = matrix_parameter * matrix_parameter;
    assert(result_matrix.getValues().is_same(op::DynamicMatrix<double>({{7., 10.}, {15., 22.}})));
    scalar = 1.;
    assert(result_matrix.getValues().is_same(op::DynamicMatrix<double>({{3., 10.}, {5., 18.}})));
    scalar = 3.;

    // multiply 2x1/2x2
    op::Parameter vector_parameter({{1., 2.}});
    result_matrix = vector_parameter * matrix_parameter;
    assert(result_matrix.getValues().is_same(op::DynamicMatrix<double>({{7., 10.}})));

    // multiply 1x1/2x2
    result_matrix = scalar_param_2 * matrix_parameter;
    assert(result_matrix.getValues().is_same(op::DynamicMatrix<double>({{2., 4.}, {6., 8.}})));

    // multiply 2x2/1x1
    result_matrix = matrix_parameter * scalar_param_2;
    assert(result_matrix.getValues().is_same(op::DynamicMatrix<double>({{2., 4.}, {6., 8.}})));

    // #ifdef EIGEN_AVAILABLE
    Eigen::Matrix3d m1, m2;
    m1.setRandom();
    m2.setRandom();
    op::Parameter eigen1(&m1);
    op::Parameter eigen2(m2);

    Eigen::MatrixXd m = scalar * m1 * m2;
    op::Parameter result = scalar_param_ptr * eigen1 * eigen2;

    assert(compareMatrices(result, m));

    scalar = 5.;
    m1.setRandom();
    m = scalar * m1 * m2;

    assert(compareMatrices(result, m));

    Eigen::MatrixXd m3x2(3, 2);
    Eigen::MatrixXd m2x5(2, 5);
    m3x2.setRandom();
    m2x5.setRandom();

    m = m3x2 * m2x5;
    result = op::Parameter(m3x2) * op::Parameter(m2x5);

    assert(compareMatrices(result, m));
    // #endif

    std::cout << "All tests were successful."
              << "\n";
}