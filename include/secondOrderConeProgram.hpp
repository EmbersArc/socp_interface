#pragma once

#include "optimizationProblem.hpp"

namespace op
{

struct SecondOrderConeProgram : public GenericOptimizationProblem
{
    std::vector<EqualityConstraint> equalityConstraints;
    std::vector<PositiveConstraint> PositiveConstraints;
    std::vector<SecondOrderConeConstraint> secondOrderConeConstraints;
    AffineExpression costFunction = Parameter();

    void addConstraint(EqualityConstraint c);
    void addConstraint(PositiveConstraint c);
    void addConstraint(SecondOrderConeConstraint c);
    void addMinimizationTerm(const AffineExpression &c);
    void printProblem(std::ostream &out) const;
    bool feasibilityCheck(const std::vector<double> &soln_values) const;
};

} // namespace op