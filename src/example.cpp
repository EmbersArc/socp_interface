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
    Eigen::MatrixXd m1_eigen(20,30);
    m1_eigen.setIdentity();
    Eigen::MatrixXd m2_eigen(30,20);
    m2_eigen.setIdentity();

    op::ParameterMatrix m1(&m1_eigen);
    op::ParameterMatrix m2(&m2_eigen);

    std::cout << "m_1_eigen: \n"
              << m1_eigen << "\n\n";
    std::cout << "m2_eigen:\n"
              << m2_eigen << "\n\n";
    std::cout << "result_eigen:\n"
              << m1_eigen * m2_eigen << "\n\n";

    op::ParameterMatrix result = m1 * m2;
}