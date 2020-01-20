# socp_interface

A C++ interface to formulate Second Order Cone Problems for the [ECOS](https://github.com/embotech/ecos) solver.

## Dependencies

* C++17
* Eigen
* ECOS (included as submodule)

## Installation
``` 
git clone --recurse-submodules https://github.com/EmbersArc/socp_interface.git
cd socp_interface  
mkdir build  
cd build  
cmake ..  
make  
```

## Example

```cpp
#include "socpSolver.hpp"

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
    op::SecondOrderConeProgram socp;

    // Add variables. Those can be scalars, vectors or matrices.
    op::Variable x = socp.createVariable("x", n);

    // Add constraints.
    for (size_t i = 0; i < m; i++)
    {
        socp.addConstraint(op::Norm2(op::Parameter(A[i]) * x + op::Parameter(b[i])) <=
                           op::Parameter(c[i]).transpose() * x + op::Parameter(d[i]));
    }
    socp.addConstraint(op::Parameter(F) * x == op::Parameter(g));

    // Here we use a pointer to a parameter. This allows changing it dynamically.
    socp.addMinimizationTerm(op::Parameter(&f).transpose() * x);

    // Print the problem for inspection.
    std::cout << socp << "\n\n";

    // Create the solver instance.
    op::Solver solver(socp);

    auto t = std::chrono::high_resolution_clock::now();
    auto t_setup = std::chrono::duration_cast<std::chrono::microseconds>(t - t0).count();
    std::cout << "\nSetup duration: " << t_setup << "μs.\n\n";

    // Solve the problem and show solver output.
    t0 = std::chrono::high_resolution_clock::now();
    solver.solveProblem(true);

    // Check if the solver has produced a valid solution.
    assert(socp.isFeasible());

    t = std::chrono::high_resolution_clock::now();
    auto t_solve = std::chrono::duration_cast<std::chrono::microseconds>(t - t0).count();
    std::cout << "\nSolver duration: " << t_solve << "μs.\n\n";

    // Get Solution.
    Eigen::Matrix<double, n, 1> x_sol;
    socp.readSolution("x", x_sol);

    // Print the first solution.
    std::cout << "Solution 1:\n"
              << x_sol << "\n\n";

    // Change the problem parameters and solve again.
    f.setRandom();
    solver.solveProblem(false);
    socp.readSolution("x", x_sol);

    // Print the new solution.
    std::cout << "Solution 2:\n"
              << x_sol << "\n\n";
}
```

