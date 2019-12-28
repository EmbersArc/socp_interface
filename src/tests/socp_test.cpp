#include "activeSolver.hpp"

#include <iostream>
#include <Eigen/Dense>

// This example solves a simple random second order cone problem
// based on https://www.cvxpy.org/examples/basic/socp.html

int main()
{
    // Set up problem data
    const size_t n = 3;
    const size_t p = 3;
    const size_t n_i = 3;

    Eigen::VectorXd f(n);
    f.setRandom();

    Eigen::VectorXd x0(n);
    x0.setRandom();

    Eigen::MatrixXd A(n_i, n);
    A.setRandom();
    Eigen::VectorXd b(n_i);
    b.setRandom();
    Eigen::VectorXd c(n_i);
    c.setRandom();
    double d = (A * x0).norm() - c.dot(x0);

    std::cout << "A :\n"
              << A << "\n\n"
              << "b :\n"
              << b << "\n\n"
              << "c :\n"
              << c << "\n\n"
              << "d :\n"
              << d << "\n\n";

    Eigen::MatrixXd F(p, n);
    F.setRandom();
    Eigen::VectorXd g = F * x0;

    std::cout << "F :\n"
              << F << "\n\n"
              << "g :\n"
              << g << "\n\n";

    // Formulate SOCP
    op::SecondOrderConeProgram socp;

    op::Variable x = socp.createVariable("x", n);

    socp.addConstraint(op::Norm2(op::Parameter(A) * x + op::Parameter(b)) <=
                       op::Parameter(c.transpose()) * x + op::Parameter(d));

    socp.addConstraint(op::Parameter(F) * x + op::Parameter(-g) == 0.);

    socp.addMinimizationTerm((op::Parameter(f.transpose()) * x)());

    std::cout << socp << std::endl;

    // Solve SOCP
    op::Solver solver(socp);

    solver.solveProblem();

    // Get Solution
    Eigen::MatrixXd x_sol;
    socp.readSolution(x, x_sol);

    std::cout << "Solution:\n"
              << x_sol << "\n\n";
    std::cout << "\n";
    std::cout << "Expected solution:\n"
              << x0 << "\n\n";
}