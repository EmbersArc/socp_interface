#include "secondOrderConeProgram.hpp"

#include <iostream>
#include <cmath>

namespace op
{

void SecondOrderConeProgram::addConstraint(std::vector<EqualityConstraint> constraints)
{
    equalityConstraints.insert(equalityConstraints.end(), constraints.begin(), constraints.end());
}

void SecondOrderConeProgram::addConstraint(std::vector<PositiveConstraint> constraints)
{
    positiveConstraints.insert(positiveConstraints.end(), constraints.begin(), constraints.end());
}

void SecondOrderConeProgram::addConstraint(std::vector<SecondOrderConeConstraint> constraints)
{
    secondOrderConeConstraints.insert(secondOrderConeConstraints.end(), constraints.begin(), constraints.end());
}

void SecondOrderConeProgram::addMinimizationTerm(const AffineSum &affine)
{
    costFunction += affine;
}

void SecondOrderConeProgram::addMinimizationTerm(const Affine &affine)
{
    assert(affine.is_scalar());
    costFunction += affine.coeff(0);
}

std::ostream &operator<<(std::ostream &os, const SecondOrderConeProgram &socp)
{
    os << "Second order cone problem with " << socp.solution_vector.size() << " variables.\n";

    os << "Number of equality constraints:          " << socp.equalityConstraints.size() << "\n";
    os << "Number of positive constraints:          " << socp.positiveConstraints.size() << "\n";
    os << "Number of second order cone constraints: " << socp.secondOrderConeConstraints.size() << "\n";

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
        for (const auto &positiveConstraint : socp.positiveConstraints)
        {
            os << positiveConstraint << "\n";
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

void SecondOrderConeProgram::cleanUp()
{
    for (auto &equalityConstraint : equalityConstraints)
    {
        equalityConstraint.affine.clean();
    }
    for (auto &positiveConstraint : positiveConstraints)
    {
        positiveConstraint.affine.clean();
    }
    for (auto &secondOrderConeConstraint : secondOrderConeConstraints)
    {
        { // Affine
            secondOrderConeConstraint.affine.clean();
        }

        { // Norm2
            secondOrderConeConstraint.affine.clean();
            for (auto &affineSum : secondOrderConeConstraint.norm2.arguments)
            {
                affineSum.clean();
            }
        }
    }

    equalityConstraints.erase(
        std::remove_if(equalityConstraints.begin(),
                       equalityConstraints.end(),
                       [](const EqualityConstraint &constraint) { return constraint.affine.is_constant(); }),
        equalityConstraints.end());

    positiveConstraints.erase(
        std::remove_if(positiveConstraints.begin(),
                       positiveConstraints.end(),
                       [](const PositiveConstraint &constraint) { return constraint.affine.is_constant(); }),
        positiveConstraints.end());

    secondOrderConeConstraints.erase(
        std::remove_if(secondOrderConeConstraints.begin(),
                       secondOrderConeConstraints.end(),
                       [](const SecondOrderConeConstraint &constraint) {
                           constraint.affine.is_constant();
                           bool all_terms_constant = true;
                           for (const AffineSum &affineSum : constraint.norm2.arguments)
                           {
                               all_terms_constant &= affineSum.is_constant();
                           }
                           return all_terms_constant;
                       }),
        secondOrderConeConstraints.end());
}

} // namespace op