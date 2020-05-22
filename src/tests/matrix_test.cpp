#include "scalar.hpp"
#include <iostream>

int main()
{
    op::MatrixXe par_matrix = op::createParameter(Eigen::Matrix3d::Identity());
    op::MatrixXe var_matrix = op::createVariables("x", 3, 3);

    op::MatrixXe matrix = (par_matrix + var_matrix).colwise().squaredNorm();

    std::cout << matrix << "\n";
}