#include "activeSolver.hpp"

#include <iostream>
#include <Eigen/Dense>

// This example solves a simple second order cone problem

int main()
{
    // op::SecondOrderConeProgram socp;

    // op::Variable v_sigma = socp.createVariableScalar();
    // op::Variable v_Delta = socp.createVariableVector(K);
    // op::Variable v_X = socp.createVariableMatrix(Model::state_dim, td.n_X());
    // op::Variable v_U = socp.createVariableMatrix(Model::input_dim, td.n_U());

    // Scalar:
    // double param = 1.;

    // op::Parameter scalar_parameter_constant(1.0);
    // op::Parameter scalar_parameter_pointer(&param);
    // op::Parameter scalar_parameter_callback([&param](){return 2. * param;});

    // op::Parameter operation1 = scalar_parameter_constant - scalar_parameter_pointer + scalar_parameter_callback;

    // std::cout << operation1.get_value() << "\n";

    // param = 5.;
    // std::cout << operation1.get_value() << "\n";

    // Matrix:
    // double param = 2.;

    // op::Parameter one(1.);
    // op::Parameter two(&param);
    // op::Parameter three(3.);
    // op::Parameter four(4.);
    // op::Parameter five(5.);
    // op::Parameter six(6.);

    // op::ParameterMatrix m1({{one, two, three}, {four, five, six}});
    // op::ParameterMatrix m2({{three, four}, {five, six}, {one, two}});

    // Eigen::MatrixXd m1_eigen(2, 3);
    // m1_eigen << 1, 2, 3, 4, 5, 6;
    // Eigen::MatrixXd m2_eigen(3, 2);
    // m2_eigen << 3, 4, 5, 6, 1, 2;

    // std::cout << m1_eigen << "\n\n";
    // std::cout << m2_eigen << "\n\n";
    // std::cout << m1_eigen * m2_eigen << "\n\n";

    // op::ParameterMatrix result = m1 * m2;

    // std::cout << result(0, 0).get_value() << result(0, 1).get_value() << "\n";
    // std::cout << result(1, 0).get_value() << result(1, 1).get_value() << "\n";

    // param *= 2;

    // m1_eigen << 1, param, 3, 4, 5, 6;
    // m2_eigen << 3, 4, 5, 6, 1, param;
    // std::cout << m1_eigen * m2_eigen << "\n\n";

    // std::cout << result(0, 0).get_value() << result(0, 1).get_value() << "\n";
    // std::cout << result(1, 0).get_value() << result(1, 1).get_value() << "\n";
}