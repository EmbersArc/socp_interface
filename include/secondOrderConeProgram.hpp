#pragma once

#include "optimizationProblem.hpp"

#include <vector>
#include <ostream>

namespace op
{

struct SecondOrderConeProgram : public GenericOptimizationProblem
{
    std::vector<SecondOrderConeConstraint> secondOrderConeConstraints;
    std::vector<PostiveConstraint> postiveConstraints;
    std::vector<EqualityConstraint> equalityConstraints;
    AffineExpression costFunction = Parameter();

    void addConstraint(SecondOrderConeConstraint c);
    void addConstraint(PostiveConstraint c);
    void addConstraint(EqualityConstraint c);
    void addMinimizationTerm(const AffineExpression &c);
    void printProblem(std::ostream &out) const;

    bool feasibilityCheck(const std::vector<double> &soln_values) const;
};

} // namespace op