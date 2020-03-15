#include "socpSolver.hpp"

#include <array>
#include <iostream>
#include <chrono>

#include <Eigen/Dense>

// This example solves the portfolio optimization problem

int main()
{
    std::vector<double> solve_times;

    // assets, factors pair
    std::vector<std::pair<size_t, size_t>> sets = {{100, 5},
                                                   {300, 10},
                                                   {500, 20},
                                                   {1000, 30},
                                                   {2000, 40},
                                                   {4000, 50},
                                                   {7500, 60},
                                                   {10000, 70}};
    for (auto [n, m] : sets)
    {
        // Set up problem data.
        double gamma = 0.5;      // risk aversion parameter
        Eigen::VectorXd mu(n);   // vector of expected returns
        Eigen::MatrixXd F(n, m); // factor-loading matrix
        Eigen::VectorXd D(n);    // diagonal of idiosyncratic risk

        mu.setRandom();
        F.setRandom();
        D.setRandom();
        mu = mu.cwiseAbs();
        F = F.cwiseAbs().transpose();
        D = D.cwiseAbs().cwiseSqrt();

        // Formulate SOCP.
        auto t0 = std::chrono::high_resolution_clock::now();

        op::SecondOrderConeProgram socp;

        op::Variable x = socp.createVariable("x", n);
        op::Variable t = socp.createVariable("t");
        op::Variable s = socp.createVariable("s");
        op::Variable u = socp.createVariable("u");
        op::Variable v = socp.createVariable("v");

        socp.addConstraint(x >= 0.);
        socp.addConstraint(op::sum(x) == op::Parameter(1.));
        socp.addConstraint(op::norm2(op::Parameter(&D).cwiseProduct(x)) <= u);
        socp.addConstraint(op::norm2(op::Parameter(&F) * x) <= v);
        socp.addConstraint(op::norm2(op::vstack({op::Parameter(1.) + -t, op::Parameter(2.) * u})) <= op::Parameter(1.) + t);
        socp.addConstraint(op::norm2(op::vstack({op::Parameter(1.) + -s, op::Parameter(2.) * v})) <= op::Parameter(1.) + s);

        socp.addMinimizationTerm(-op::Parameter(&mu).transpose() * x);
        socp.addMinimizationTerm(op::Parameter(gamma) * (t + s));

        // Create and initialize the solver instance.
        op::Solver solver(socp);
        solver.initialize();

        // Solve the problem and show solver output.
        const size_t repetitions = 100;
        double total_time = 0.;
        for (size_t rep = 0; rep < repetitions; rep++)
        {
            mu.setRandom();
            F.setRandom();
            D.setRandom();
            mu = mu.cwiseAbs();
            F = F.cwiseAbs().transpose();
            D = D.cwiseAbs().cwiseSqrt();

            t0 = std::chrono::high_resolution_clock::now();
            const bool success = solver.solveProblem(false);
            auto t1 = std::chrono::high_resolution_clock::now();
            total_time += std::chrono::duration<double>(t1 - t0).count();
        }
        solve_times.push_back(total_time / repetitions);
    }

    for (size_t i = 0; i < sets.size(); i++)
    {
        std::cout << sets[i].first << "," << sets[i].second << "," << solve_times[i] << "\n";
    }
}