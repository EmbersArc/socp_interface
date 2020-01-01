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
    positiveConstraints.push_back(constraint);
}

void SecondOrderConeProgram::addConstraint(std::vector<EqualityConstraint> constraints)
{
    equalityConstraints.insert(equalityConstraints.end(), constraints.begin(), constraints.end());
}

void SecondOrderConeProgram::addConstraint(std::vector<PositiveConstraint> constraints)
{
    positiveConstraints.insert(positiveConstraints.end(), constraints.begin(), constraints.end());
}

void SecondOrderConeProgram::addConstraint(SecondOrderConeConstraint constraint)
{
    secondOrderConeConstraints.push_back(constraint);
}

void SecondOrderConeProgram::addMinimizationTerm(const AffineSum &affine)
{
    costFunction = costFunction + affine;
}

void SecondOrderConeProgram::addMinimizationTerm(const Affine &affine)
{
    assert(affine.is_scalar());
    costFunction = costFunction + affine.coeff(0);
}

std::ostream &operator<<(std::ostream &os, const SecondOrderConeProgram &socp)
{
    os << "Minimize:"
       << "\n";
    os << socp.costFunction << "\n";

    if (not socp.equalityConstraints.empty())
    {
        os << "\n"
           << "Subject to equality constraints:"
           << "\n";
        for (const auto &equalityConstraint : socp.equalityConstraints)
        {
            os << equalityConstraint << "\n";
        }
    }

    if (not socp.positiveConstraints.empty())
    {
        os << "\n"
           << "Subject to linear inequalities:"
           << "\n";
        for (const auto &PositiveConstraint : socp.positiveConstraints)
        {
            os << PositiveConstraint << "\n";
        }
    }

    if (not socp.secondOrderConeConstraints.empty())
    {
        os << "\n"
           << "Subject to cone constraints:"
           << "\n";
        for (const auto &secondOrderConeConstraint : socp.secondOrderConeConstraints)
        {
            os << secondOrderConeConstraint << "\n";
        }
    }
    return os;
}

template <typename T>
bool check_constraint(double tol, double val, const T &constraint)
{
    if (val > tol)
    {
        std::cout << "Infeasible solution, constraint value: " + std::to_string(val) << "\n"
                  << constraint << "\n";

        return false;
    }
    return true;
}

bool SecondOrderConeProgram::isFeasible() const
{
    const double tol = 0.01;
    bool feasible = true;
    auto check = [&](const auto &constraint) { return check_constraint(tol,
                                                                       constraint.evaluate(solution_vector),
                                                                       constraint); };
    auto check_abs = [&](const auto &constraint) { return check_constraint(tol,
                                                                           std::fabs(constraint.evaluate(solution_vector)),
                                                                           constraint); };

    feasible &= std::all_of(secondOrderConeConstraints.begin(),
                            secondOrderConeConstraints.end(),
                            check);
    feasible &= std::all_of(positiveConstraints.begin(),
                            positiveConstraints.end(),
                            check);
    feasible &= std::all_of(equalityConstraints.begin(),
                            equalityConstraints.end(),
                            check_abs);

    return feasible;
}

} // namespace op