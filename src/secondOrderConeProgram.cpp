#include "secondOrderConeProgram.hpp"

#include <iostream>
#include <cmath>

namespace op
{

void SecondOrderConeProgram::addConstraint(EqualityConstraint constraint)
{
    equalityConstraints.push_back(constraint);
}

void SecondOrderConeProgram::addConstraint(PositiveConstraint constraint)
{
    PositiveConstraints.push_back(constraint);
}

void SecondOrderConeProgram::addConstraint(std::vector<EqualityConstraint> constraints)
{
    for (const EqualityConstraint &c : constraints)
        addConstraint(c);
}

void SecondOrderConeProgram::addConstraint(std::vector<PositiveConstraint> constraints)
{
    for (const PositiveConstraint &c : constraints)
        addConstraint(c);
}

void SecondOrderConeProgram::addConstraint(SecondOrderConeConstraint constraint)
{
    secondOrderConeConstraints.push_back(constraint);
}

void SecondOrderConeProgram::addMinimizationTerm(const AffineExpression &affine)
{
    costFunction = costFunction + affine;
}

void SecondOrderConeProgram::printProblem(std::ostream &out) const
{
    out << "Minimize"
        << "\n";
    out << costFunction << "\n";

    out << "\n"
        << "Subject to equality constraints"
        << "\n";
    for (const auto &equalityConstraint : equalityConstraints)
    {
        out << equalityConstraint << "\n";
    }

    out << "\n"
        << "Subject to linear inequalities"
        << "\n";
    for (const auto &PositiveConstraint : PositiveConstraints)
    {
        out << PositiveConstraint << "\n";
    }

    out << "\n"
        << "Subject to cone constraints"
        << "\n";
    for (const auto &secondOrderConeConstraint : secondOrderConeConstraints)
    {
        out << secondOrderConeConstraint << "\n";
    }
}

template <typename T>
bool feasibility_check_message(double tol, double val, const T &constraint)
{
    if (val > tol)
    {
        std::cout << "Infeasible solution, constraint value: " + std::to_string(val) << "\n"
                  << constraint << "\n";

        return false;
    }
    return true;
}

bool SecondOrderConeProgram::feasibilityCheck(const std::vector<double> &soln_values) const
{
    const double tol = 0.1;
    bool feasible = true;
    auto check = [&](const auto &constraint) { return feasibility_check_message(
                                                   tol, constraint.evaluate(soln_values), constraint); };
    auto check_abs = [&](const auto &constraint) { return feasibility_check_message(
                                                       tol, std::fabs(constraint.evaluate(soln_values)), constraint); };

    feasible &= std::all_of(secondOrderConeConstraints.begin(),
                            secondOrderConeConstraints.end(),
                            check);
    feasible &= std::all_of(PositiveConstraints.begin(),
                            PositiveConstraints.end(),
                            check);
    feasible &= std::all_of(equalityConstraints.begin(),
                            equalityConstraints.end(),
                            check_abs);

    return feasible;
}

} // namespace op