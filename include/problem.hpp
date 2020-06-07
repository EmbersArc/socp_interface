#pragma once

#include "constraint.hpp"
#include <Eigen/Sparse>

namespace op
{

    class WrapperBase;

    class OptimizationProblem
    {
    public:
        void addConstraint(std::vector<Constraint> constraints);

        void addMinimizationTerm(const Expression &term);

        bool isFeasible() const;

        size_t getNumVariables() const;

        friend std::ostream &operator<<(std::ostream &os, const OptimizationProblem &socp);
        friend WrapperBase;

    private:
        Expression costFunction;
        std::vector<EqualityConstraint> equality_constraints;
        std::vector<PositiveConstraint> positive_constraints;
        std::vector<SecondOrderConeConstraint> second_order_cone_constraints;
    };

} // namespace op