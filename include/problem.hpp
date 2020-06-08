#pragma once

#include "constraint.hpp"
#include <Eigen/Sparse>

namespace op
{

    class WrapperBase;

    class OptimizationProblem
    {
    public:
        void addConstraint(const Constraint &constraint);
        void addConstraint(const std::vector<Constraint> &constraints);

        void addMinimizationTerm(const Expression &expression);

        bool isFeasible() const;

        size_t getNumVariables() const;

        friend std::ostream &operator<<(std::ostream &os, const OptimizationProblem &socp);
        friend WrapperBase;

        template <typename Derived>
        void addMinimizationTerm(const Eigen::MatrixBase<Derived> &expression)
        {
            if (not(expression.rows() == 1 and expression.cols() == 1))
            {
                throw std::runtime_error("Minimization term has to be a scalar!");
            }
            addMinimizationTerm(expression(0, 0));
        }

    private:
        Expression costFunction;
        std::vector<EqualityConstraint> equality_constraints;
        std::vector<PositiveConstraint> positive_constraints;
        std::vector<SecondOrderConeConstraint> second_order_cone_constraints;
    };

} // namespace op