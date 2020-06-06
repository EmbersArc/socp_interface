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

    void OptimizationProblem::finalize()
    {
        for (EqualityConstraint &c : this->equality_constraints)
        {
            for (Term &term : c.affine.terms)
            {
                addVariable(term.variable);
                this->A_coeffs.emplace_back(b_coeffs.size(),
                                            term.variable.getProblemIndex(),
                                            term.parameter);
            }

            b_coeffs.push_back(c.affine.constant);
        }

        for (PositiveConstraint &constraint : this->positive_constraints)
        {
            for (Term &term : constraint.affine.terms)
            {
                addVariable(term.variable);
                this->G_coeffs.emplace_back(h_coeffs.size(),
                                            term.variable.getProblemIndex(),
                                            term.parameter);
            }

            h_coeffs.push_back(constraint.affine.constant);
        }

        for (SecondOrderConeConstraint &constraint : this->second_order_cone_constraints)
        {
            // affine part
            for (Term &term : constraint.affine.terms)
            {
                addVariable(term.variable);
                this->G_coeffs.emplace_back(h_coeffs.size(),
                                            term.variable.getProblemIndex(),
                                            term.parameter);
            }
            h_coeffs.push_back(constraint.affine.constant);

            // norm part
            for (Affine &affine : constraint.norm)
            {
                for (Term &term : affine.terms)
                {
                    addVariable(term.variable);
                    this->G_coeffs.emplace_back(h_coeffs.size(),
                                                term.variable.getProblemIndex(),
                                                term.parameter);
                }
                h_coeffs.push_back(constraint.affine.constant);
            }
        }

        if (this->costFunction.getOrder() == 1)
        {
            throw std::runtime_error("SOCP cost functions must be linear!");
        }

        this->c.resize(n_variables);
        this->c.setZero();

        for (Term &term : this->costFunction.affine.terms)
        {
            addVariable(term.variable);
            this->c(term.variable.getProblemIndex()) = term.parameter;
        }

        this->A.setFromTriplets(this->A_coeffs.begin(), this->A_coeffs.end());
        this->G.setFromTriplets(this->G_coeffs.begin(), this->G_coeffs.end());
        this->b = Eigen::Map<VectorXp>(this->b_coeffs.data(), this->b_coeffs.size());
        this->h = Eigen::Map<VectorXp>(this->h_coeffs.data(), this->h_coeffs.size());

        solution.resize(n_variables);
    }

    void OptimizationProblem::addVariable(Variable &variable)
    {
        if (not variable.isLinkedToProblem())
        {
            variable.linkToProblem(&solution, n_variables);
            n_variables++;
        }
    }

    std::ostream &operator<<(std::ostream &os, const OptimizationProblem &)
    {
        os << "todo";
        return os;
    }

} // namespace op