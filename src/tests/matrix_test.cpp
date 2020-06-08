#include "constraint.hpp"
#include <iostream>

int main()
{
    op::MatrixXe par_matrix = op::createParameter(Eigen::Vector3d::Ones());
    op::MatrixXe var_matrix = op::createVariables("x", 3, 3);

    op::MatrixXe matrix = var_matrix*par_matrix;

    op::Constraint constraint = op::lessThan(var_matrix.norm(), op::Expression(0.));

    std::cout << constraint << "\n\n";
    std::cout << par_matrix << "\n\n";
    std::cout << var_matrix << "\n\n";
    std::cout << op::evaluate(matrix) << "\n\n";
}