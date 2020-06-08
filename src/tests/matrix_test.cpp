#include "constraint.hpp"
#include <iostream>

int main()
{
    op::MatrixXe par_matrix = op::createParameter(Eigen::Vector3d::Ones());
    op::MatrixXe var_matrix = op::createVariables("x", 3, 3);

    std::cout << par_matrix << "\n\n";
    std::cout << par_matrix.cast<double>() << "\n\n";
    // std::cout << op::evaluate(par_matrix) << "\n\n";

    op::MatrixXe matrix = par_matrix * var_matrix;

    auto constraints = op::lessThan(var_matrix.norm(), op::Expression(0.));

    // for (op::Constraint &c : constraints)
    // {
    //     std::cout << c << std::endl;
    // }

    std::cout << constraints << "\n\n";
    std::cout << par_matrix << "\n\n";
    std::cout << var_matrix << "\n\n";
}