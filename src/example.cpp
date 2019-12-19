#include "activeSolver.hpp"

#include <iostream>

// This example solves a simple second order cone problem

int main()
{
    op::SecondOrderConeProgram socp;

    // Create tensor variables:
    // solution variable vector X with dimension 3 (more dimensions can be added as needed)
    socp.createTensorVariable("X", {3 /*, 20, ...*/});
    // upper bound of 2-norm of X
    socp.createTensorVariable("norm2_X");

    // Define variables:
    double x_max = 0.;
    double y_max = -1.;
    double z_min = 2.;

    // Shortcut functions to access solver variables and create parameters
    auto var = [&socp](const std::string &name, const std::vector<size_t> &indices = {}) { return socp.getVariable(name, indices); };

    op::AffineTerm test = var("X", {0}) * 1.0;

    // ||X||
    std::vector<op::AffineExpression> norm2_args({1.0 * var("X", {0}),
                                                  1.0 * var("X", {1}),
                                                  1.0 * var("X", {2})});
    op::Norm2 norm2_X = op::norm2(norm2_args);
    // Add second order cone constraint:
    // ||X|| <= norm2_X
    socp.addConstraint(norm2_X <= 1.0 * var("norm2_X"));

    // Add linear constraints:
    // x_max >= X(0)
    // y_max >= Y(1)
    // z_min <= Z(2)
    socp.addConstraint(-1.0 * var("X", {0}) + op::Parameter(&x_max) >= 0.);
    socp.addConstraint(-1.0 * var("X", {1}) + op::Parameter(&y_max) >= 0.);
    socp.addConstraint(1.0 * var("X", {2}) + -op::Parameter(&z_min) >= 0.);

    // Objective:
    // minimize ||X||
    socp.addMinimizationTerm(1.0 * var("norm2_X"));

    // Create solver
    Solver solver(socp);

    // Solve first problem
    solver.solveProblem(true); // pass false for quiet solver

    // Print solution variables
    std::cout << "Result:\n"
              << "X:  " << solver.getSolutionValue("X", {0}) << '\n'
              << "Y:  " << solver.getSolutionValue("X", {1}) << '\n'
              << "Z:  " << solver.getSolutionValue("X", {2}) << '\n';

    // Change parameters
    x_max = -1.;
    y_max = -2.;
    z_min = 3.;

    // Solve second problem with new parameters
    solver.solveProblem(true);

    // It is also possible to index variables for performance
    const unsigned int X0_index = socp.getTensorVariableIndex("X", {0});
    const unsigned int X1_index = socp.getTensorVariableIndex("X", {1});
    const unsigned int X2_index = socp.getTensorVariableIndex("X", {2});
    std::cout << "Result:\n"
              << "X:  " << solver.getSolutionValue(X0_index) << '\n'
              << "Y:  " << solver.getSolutionValue(X1_index) << '\n'
              << "Z:  " << solver.getSolutionValue(X2_index) << '\n';
}