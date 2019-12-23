#include "activeSolver.hpp"

#include <iostream>
#include <Eigen/Dense>

// This example solves a simple second order cone problem

int main()
{
    op::SecondOrderConeProgram socp;

    op::Variable v_a = socp.createVariable("a");
    // op::VariableMatrix v_M = socp.createTensorVariable("M", {5, 5});

    double par = 1.0;
    op::Parameter param(par);

    socp.addConstraint(op::Parameter(1.0) * v_a + op::Parameter(-5.0) * param >= 0);
    socp.addMinimizationTerm(1.0 * v_a);

    EcosWrapper solver(socp);

    solver.solveProblem(true);

    double solution;
    socp.readSolution(v_a, solution);

    std::cout << socp << "\n";

    std::cout << solution << "\n";

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
    // Eigen::MatrixXd m1_eigen(20,30);
    // m1_eigen.setIdentity();
    // Eigen::MatrixXd m2_eigen(30,20);
    // m2_eigen.setIdentity();

    // op::ParameterMatrix m1(&m1_eigen);
    // op::ParameterMatrix m2(&m2_eigen);

    // std::cout << "m_1_eigen: \n"
    //           << m1_eigen << "\n\n";
    // std::cout << "m2_eigen:\n"
    //           << m2_eigen << "\n\n";
    // std::cout << "result_eigen:\n"
    //           << m1_eigen * m2_eigen << "\n\n";

    // op::ParameterMatrix result = m1 * m2;
}