#include "expression.hpp"
#include <iostream>

int main()
{
    op::MatrixXe par_matrix = op::parameterMatrix(Eigen::Matrix3d::Identity());
    op::MatrixXe var_matrix = op::createVariables("x", 3, 3);

    op::MatrixXe matrix = (var_matrix).rowwise().norm();

    std::cout << matrix << "\n";
}