#include "expression.hpp"
#include <iostream>

int main()
{
    op::MatrixXe par_matrix = op::parameterMatrix(Eigen::Matrix3d::Identity());
    op::MatrixXe var_matrix = op::createVariables("x", 3, 3);

    op::MatrixXe matrix = par_matrix * var_matrix;

    std::cout << "Shape: "
              << "(" << matrix.rows() << "," << matrix.cols() << ")"
              << "\n\n";
    std::cout << matrix << "\n";
}