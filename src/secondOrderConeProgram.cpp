#include "secondOrderConeProgram.hpp"

#include "constraint.hpp"

#include <iostream>
#include <cmath>

namespace op
{

void SecondOrderConeProgram::addConstraint(SecondOrderConeConstraint c)
{
    secondOrderConeConstraints.push_back(c);
}

void SecondOrderConeProgram::addConstraint(PostiveConstraint c)
{
    postiveConstraints.push_back(c);
}

void SecondOrderConeProgram::addConstraint(EqualityConstraint c)
{
    equalityConstraints.push_back(c);
}

void SecondOrderConeProgram::addMinimizationTerm(const AffineExpression &c)
{
    costFunction = costFunction + c;
}

void SecondOrderConeProgram::printProblem(std::ostream &out) const
{
    out << "Minimize"
        << "\n";
    out << costFunction.print() << "\n";

    out << "\n"
        << "Subject to equality constraints"
        << "\n";
    for (const auto &equalityConstraint : equalityConstraints)
    {
        out << equalityConstraint.print() << "\n";
    }

    out << "\n"
        << "Subject to linear inequalities"
        << "\n";
    for (const auto &postiveConstraint : postiveConstraints)
    {
        out << postiveConstraint.print() << "\n";
    }

    out << "\n"
        << "Subject to cone constraints"
        << "\n";
    for (const auto &secondOrderConeConstraint : secondOrderConeConstraints)
    {
        out << secondOrderConeConstraint.print() << "\n";
    }
}

template <typename T>
bool feasibility_check_message(double tol, double val, const T &constraint)
{
    if (val > tol)
    {
        std::cout << "Infeasible solution, constraint value: " + std::to_string(val) << "\n"
                  << constraint.print() << "\n";

        return false;
    }
    return true;
}

bool SecondOrderConeProgram::feasibilityCheck(const std::vector<double> &soln_values) const
{
    const double tol = 0.1;
    bool feasible = true;
    auto check_feasibility = [&](const auto &constraint) { return feasibility_check_message(tol, constraint.evaluate(soln_values), constraint); };
    auto check_feasibility_abs = [&](const auto &constraint) { return feasibility_check_message(tol, std::fabs(constraint.evaluate(soln_values)), constraint); };

    feasible &= std::all_of(secondOrderConeConstraints.begin(), secondOrderConeConstraints.end(), check_feasibility);
    feasible &= std::all_of(postiveConstraints.begin(), postiveConstraints.end(), check_feasibility);
    feasible &= std::all_of(equalityConstraints.begin(), equalityConstraints.end(), check_feasibility_abs);

    return feasible;
}

} // namespace op