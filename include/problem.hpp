#pragma once

#include "constraint.hpp"
#include <Eigen/Sparse>

namespace cvx
{

    class WrapperBase;

    class OptimizationProblem
    {
    public:
        void addConstraint(const Constraint &constraint);
        void addConstraint(const std::vector<Constraint> &constraints);

        void addMinimizationTerm(const Scalar &term);

        bool isFeasible() const;

        size_t getNumVariables() const;

        friend std::ostream &operator<<(std::ostream &os, const OptimizationProblem &socp);
        friend WrapperBase;

    private:
        Scalar costFunction;
        std::vector<EqualityConstraint> equality_constraints;
        std::vector<PositiveConstraint> positive_constraints;
        std::vector<SecondOrderConeConstraint> second_order_cone_constraints;
    };

} // namespace cvx