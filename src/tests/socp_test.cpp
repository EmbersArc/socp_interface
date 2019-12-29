#include "activeSolver.hpp"

#include <array>
#include <iostream>
#include <chrono>

#include <Eigen/Dense>

// This example solves a simple random second order cone problem
// based on https://www.cvxpy.org/examples/basic/socp.html

int main()
{
    auto t1 = std::chrono::high_resolution_clock::now();

    // Set up problem data

    // number of second order cone constraints
    const size_t m = 3;
    // number of variables
    const size_t n = 10;
    // dimension of equality constraints
    const size_t p = 5;
    // dimension of second order cone constraints
    const size_t n_i = 5;

    std::array<Eigen::Matrix<double, n_i, n>, m> A;
    std::array<Eigen::Matrix<double, n_i, 1>, m> b;
    std::array<Eigen::Matrix<double, n, 1>, m> c;
    std::array<double, m> d;

    Eigen::Matrix<double, n, 1> x0;
    x0.setRandom();
    Eigen::Matrix<double, n, 1> f;
    f.setRandom();

    for (size_t i = 0; i < m; i++)
    {
        A[i].setRandom();
        b[i].setRandom();
        c[i].setRandom();
        d[i] = (A[i] * x0).norm() - c[i].dot(x0);
    }

    // std::cout << "A :\n"
    //           << A << "\n\n"
    //           << "b :\n"
    //           << b << "\n\n"
    //           << "c :\n"
    //           << c << "\n\n"
    //           << "d :\n"
    //           << d << "\n\n";

    Eigen::Matrix<double, p, n> F;
    F.setRandom();
    Eigen::Matrix<double, p, 1> g = F * x0;

    // std::cout << "F :\n"
    //           << F << "\n\n"
    //           << "g :\n"
    //           << g << "\n\n";

    // Formulate SOCP
    op::SecondOrderConeProgram socp;

    op::Variable x = socp.createVariable("x", n);

    for (size_t i = 0; i < m; i++)
    {
        socp.addConstraint(op::Norm2(op::Parameter(A[i]) * x + op::Parameter(b[i])) <=
                           op::Parameter(c[i]).transpose() * x + op::Parameter(d[i]));
    }
    socp.addConstraint(op::Parameter(F) * x + op::Parameter(-g) == 0.);
    socp.addMinimizationTerm(op::Parameter(f).transpose() * x);
    std::cout << socp << "\n\n";

    // Solve SOCP
    op::Solver solver(socp);

    solver.solveProblem(true);

    assert(socp.isFeasible());

    // Get Solution
    Eigen::Matrix<double, n, 1> solution;
    socp.readSolution(x, solution);

    std::cout << "Solution:\n"
              << solution << "\n\n";
    std::cout << "\n";
    std::cout << "Expected solution:\n"
              << x0 << "\n\n";

    const double error_norm = (x0 - solution).norm();

    std::cout << "Error: " << error_norm << "\n\n";

    assert(error_norm < 1e-2);

    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

    std::cout << "All tests were successful (" << duration << "μs).\n";
}