#include "eicosWrapper.hpp"

#include <array>
#include <iostream>
#include <chrono>

#include <Eigen/Dense>

// This example solves a simple random second order cone problem
// based on https://www.cvxpy.org/examples/basic/socp.html

int main()
{
    // Set up problem data.

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

    Eigen::Matrix<double, p, n> F;
    F.setRandom();
    Eigen::Matrix<double, p, 1> g = F * x0;

    // Formulate SOCP.
    auto t0 = std::chrono::high_resolution_clock::now();

    // Create the SOCP instance.
    op::OptimizationProblem socp;

    // Add variables. Those can be scalars, vectors or matrices.
    op::VectorXe x = op::createVariables("x", n);

    // Add constraints.
    // SOCP
    for (size_t i = 0; i < m; i++)
    {
        op::Expression lhs = (op::createParameter(A[i]) * x + op::createParameter(b[i])).norm();
        op::Expression rhs = op::createParameter(c[i]).transpose().dot(x) + op::createParameter(d[i]);
        auto constraint = op::lessThan(lhs, rhs);
        socp.addConstraint(constraint);
    }
    // Equality
    socp.addConstraint(op::equalTo(op::createParameter(F) * x, op::createParameter(g)));

    // Here we use a pointer to a parameter. This allows changing it dynamically.
    socp.addMinimizationTerm(op::createParameter(&f).transpose() * x);

    // Print the problem for inspection.
    std::cout << socp << "\n\n";

    // Create the solver instance.
    op::EicosWrapper solver(socp);
    solver.initialize();

    auto t = std::chrono::high_resolution_clock::now();
    auto t_setup = std::chrono::duration_cast<std::chrono::microseconds>(t - t0).count();
    std::cout << "\nSetup duration: " << t_setup << "μs.\n\n";

    // Solve the problem and show solver output.
    t0 = std::chrono::high_resolution_clock::now();
    const bool success = solver.solveProblem(true);
    if (not success)
    {
        // This should not happen in this example.
        throw std::runtime_error("Solver returned a critical error.");
    }
    std::cout << "Solver message: " << solver.getResultString() << "\n";

    // Check if the solver has produced a valid solution.
    assert(socp.isFeasible());

    t = std::chrono::high_resolution_clock::now();
    auto t_solve = std::chrono::duration_cast<std::chrono::microseconds>(t - t0).count();
    std::cout << "\nSolver duration: " << t_solve << "μs.\n\n";

    // Get Solution.
    Eigen::Matrix<double, n, 1> x_sol = x.cast<double>();

    // Print the first solution.
    std::cout << "First solution:\n"
              << x_sol << "\n\n";

    // First solution:
    // 0.859947
    // 0.0760788
    // 0.0157263
    // 0.00550263
    // -0.537155
    // -0.0344149
    // -0.676446
    // 0.0850391
    // 0.475255
    // -1.13711

    // Change the problem parameters and solve again.
    f.setRandom();
    solver.solveProblem(false);
    x_sol = x.cast<double>();

    // Print the new solution.
    std::cout << "Solution after changing the cost function:\n"
              << x_sol << "\n\n";

    // Solution after changing the cost function:
    // 1.04428
    // 0.37218
    // -0.389004
    // 0.0957543
    // -0.135829
    // 1.04661
    // -0.459681
    // 0.126881
    // 0.794843
    // -0.840825

}