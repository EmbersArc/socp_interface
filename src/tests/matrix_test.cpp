#include "constraint.hpp"
#include <iostream>

int main()
{
    op::MatrixX par_matrix = op::par(Eigen::Vector3d::Ones());
    op::MatrixX var_matrix = op::var("x", 3, 3);

    op::MatrixX matrix = var_matrix*par_matrix;

    op::Constraint constraint = op::lessThan(var_matrix.norm(), op::Scalar(0.));

    std::cout << constraint << "\n\n";
    std::cout << par_matrix << "\n\n";
    std::cout << var_matrix << "\n\n";
    std::cout << op::eval(matrix) << "\n\n";
}