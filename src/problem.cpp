#include "problem.hpp"

namespace op
{

    void OptimizationProblem::addConstraint(std::vector<Constraint> constraints)
    {
        for (const Constraint &constraint : constraints)
        {
            if (constraint.getType() == Constraint::Type::Equality)
            {
                this->equality_constraints.push_back(std::get<Constraint::Type::Equality>(constraint.data));
            }
            else if (constraint.getType() == Constraint::Type::Positive)
            {
                this->positive_constraints.push_back(std::get<Constraint::Type::Positive>(constraint.data));
            }
            else if (constraint.getType() == Constraint::Type::SecondOrderCone)
            {
                this->second_order_cone_constraints.push_back(std::get<Constraint::Type::SecondOrderCone>(constraint.data));
            }
        }
    }

    void OptimizationProblem::addMinimizationTerm(const Expression &term)
    {
        this->costFunction += term;
    }

    bool OptimizationProblem::isFeasible() const
    {
        // TODO
        return true;
    }

    std::ostream &operator<<(std::ostream &os, const OptimizationProblem &)
    {
        os << "todo";
        return os;
    }

} // namespace op