#pragma once

#include "optimizationProblem.hpp"

namespace op
{

struct SecondOrderConeProgram : public GenericOptimizationProblem
{
    std::vector<EqualityConstraint> equalityConstraints;
    std::vector<PositiveConstraint> positiveConstraints;
    std::vector<SecondOrderConeConstraint> secondOrderConeConstraints;
    internal::AffineSum costFunction = internal::AffineSum(0.);

    void addConstraint(std::vector<EqualityConstraint> constraints);
    void addConstraint(std::vector<PositiveConstraint> constraints);
    void addConstraint(std::vector<SecondOrderConeConstraint> constraints);

    void addMinimizationTerm(const Affine &affine);

    void cleanUp();

    bool isFeasible() const;

    friend std::ostream &operator<<(std::ostream &os, const SecondOrderConeProgram &socp);
};

} // namespace op