#include "activeSolver.hpp"

#include <iostream>

// this example solves a simple second order cone problem

int main()
{
    op::SecondOrderConeProgram socp;

    // create tensor variables:
    // solution variable vector X
    socp.createTensorVariable("X", {3});
    // ||X||
    socp.createTensorVariable("norm2_X");

    // define variables
    double x_max = 0.;
    double y_max = -1.;
    double z_min = 2.;

    // shortcuts to access solver variables and create parameters
    auto var = [&socp](const std::string &name, const std::vector<size_t> &indices = {}) { return socp.getVariable(name, indices); };
    auto param = [](double &param_value) { return op::Parameter(&param_value); };
    auto param_fn = [](std::function<double()> callback) { return op::Parameter(callback); };

    // ||X||
    std::vector<op::AffineExpression> norm2_args({1.0 * var("X", {0}),
                                                  1.0 * var("X", {1}),
                                                  1.0 * var("X", {2})});
    op::Norm2 norm2_X = op::norm2(norm2_args);
    // add second order cone constraint:
    // ||X|| <= norm2_X
    socp.addConstraint(norm2_X <= 1.0 * var("norm2_X"));

    // add linear constraints:
    // x_max >= X(0)
    // y_max >= Y(1)
    // z_min <= Z(2)
    socp.addConstraint(-1.0 * var("X", {0}) + param(x_max) >= 0.);
    socp.addConstraint(-1.0 * var("X", {1}) + param(y_max) >= 0.);
    // use param_fn to multiply z_min by -1.
    socp.addConstraint(1.0 * var("X", {2}) + param_fn([&z_min]() { return -1.0 * z_min; }) >= 0.);

    // objective:
    // minimize ||X||
    socp.addMinimizationTerm(1.0 * var("norm2_X"));

    // create solver
    Solver solver(socp);

    // solve first problem
    solver.solveProblem(true); // pass false for quiet solver

    // print solution variables
    std::cout << solver.getSolutionValue("X", {0}) << '\n'
              << solver.getSolutionValue("X", {1}) << '\n'
              << solver.getSolutionValue("X", {2}) << '\n';

    // change parameters
    x_max = -1.;
    y_max = -2.;
    z_min = 3.;

    // solve second problem
    solver.solveProblem(true);

    // it is also possible to index variables for performance
    const unsigned int X0_index = socp.getTensorVariableIndex("X", {0});
    const unsigned int X1_index = socp.getTensorVariableIndex("X", {1});
    const unsigned int X2_index = socp.getTensorVariableIndex("X", {2});
    std::cout << solver.getSolutionValue(X0_index) << '\n'
              << solver.getSolutionValue(X1_index) << '\n'
              << solver.getSolutionValue(X2_index) << '\n';
}