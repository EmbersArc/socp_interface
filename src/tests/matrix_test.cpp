#include "constraint.hpp"
#include <iostream>

int main()
{
    cvx::MatrixX par_matrix = cvx::par(Eigen::Vector3d::Ones());
    cvx::MatrixX var_matrix = cvx::var("x", 3, 3);

    cvx::MatrixX matrix = var_matrix*par_matrix;

    cvx::Constraint constraint = cvx::lessThan(var_matrix.norm(), cvx::Scalar(0.));

    std::cout << constraint << "\n\n";
    std::cout << par_matrix << "\n\n";
    std::cout << var_matrix << "\n\n";
    std::cout << cvx::eval(matrix) << "\n\n";
}