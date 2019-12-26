#include "activeSolver.hpp"

#include <iostream>
#include <Eigen/Dense>

// This example solves a simple random second order cone problem

int main()
{
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
    Eigen::VectorXd g = F * x0;

    std::cout << "F :\n"
              << A << "\n\n"
              << "g :\n"
              << d << "\n\n";

    std::cout << "Expected solution: " << x0 << "\n";

    op::SecondOrderConeProgram socp;

    op::VariableMatrix x = socp.createVariableMatrix("x", n);

    socp.addConstraint(op::Norm2(op::Parameter(b) + op::Parameter(A) * x) <=
                       op::Parameter(c.T) * x + op::Parameter(d));
    socp.addConstraint(op::Parameter(F) * x == op::Parameter(g));

    socp.addMinimizationTerm(op::Parameter(f) * x)
}