#pragma once

#include "optimizationProblem.hpp"

namespace op
{

struct SecondOrderConeProgram : public GenericOptimizationProblem
{
    std::vector<EqualityConstraint> equalityConstraints;
    std::vector<PositiveConstraint> positiveConstraints;
    std::vector<SecondOrderConeConstraint> secondOrderConeConstraints;
    AffineExpression costFunction = Parameter();

    void addConstraint(EqualityConstraint constraint);
    void addConstraint(PositiveConstraint constraint);
    void addConstraint(std::vector<EqualityConstraint> constraints);
    void addConstraint(std::vector<PositiveConstraint> constraints);
    void addConstraint(SecondOrderConeConstraint constraint);
    
    void addMinimizationTerm(const AffineExpression &affine);

    bool isFeasible() const;

    friend std::ostream &operator<<(std::ostream &os, const SecondOrderConeProgram &socp);
};

} // namespace op