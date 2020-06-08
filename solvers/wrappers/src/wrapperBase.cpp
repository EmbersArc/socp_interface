#include "wrapperBase.hpp"

namespace op
{

    WrapperBase::WrapperBase(OptimizationProblem &problem)
        : problem(problem)
    {
        std::vector<Eigen::Triplet<Parameter>> A_coeffs, G_coeffs;
        std::vector<Parameter> b_coeffs, h_coeffs;
        std::vector<int> cone_dimensions;

        // Build equality constraint parameters (b - A * x == 0)
        for (EqualityConstraint &c : problem.equality_constraints)
        {
            for (Term &term : c.affine.terms)
            {
                addVariable(term.variable);
                A_coeffs.emplace_back(b_coeffs.size(),
                                      term.variable.getProblemIndex(),
                                      term.parameter);
            }

            b_coeffs.push_back(c.affine.constant);
        }

        // Build positive constraint parameters
        for (PositiveConstraint &constraint : problem.positive_constraints)
        {
            for (Term &term : constraint.affine.terms)
            {
                addVariable(term.variable);
                G_coeffs.emplace_back(h_coeffs.size(),
                                      term.variable.getProblemIndex(),
                                      term.parameter);
            }

            h_coeffs.push_back(constraint.affine.constant);
        }

        // Build second order cone constraint parameters
        for (SecondOrderConeConstraint &constraint : problem.second_order_cone_constraints)
        {
            // affine part
            for (Term &term : constraint.affine.terms)
            {
                addVariable(term.variable);
                G_coeffs.emplace_back(h_coeffs.size(),
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
                    G_coeffs.emplace_back(h_coeffs.size(),
                                          term.variable.getProblemIndex(),
                                          term.parameter);
                }
                h_coeffs.push_back(affine.constant);
            }

            cone_dimensions.push_back(constraint.norm.size() + 1);
        }

        if (problem.costFunction.getOrder() != 1)
        {
            throw std::runtime_error("SOCP cost functions must be linear!");
        }

        c.resize(n_variables);
        c.setZero();

        for (Term &term : problem.costFunction.affine.terms)
        {
            addVariable(term.variable);
            c(term.variable.getProblemIndex()) = term.parameter;
        }

        A.resize(b_coeffs.size(), n_variables);
        G.resize(h_coeffs.size(), n_variables);

        A.setFromTriplets(A_coeffs.begin(), A_coeffs.end());
        G.setFromTriplets(G_coeffs.begin(), G_coeffs.end());
        b = Eigen::Map<VectorXp>(b_coeffs.data(), b_coeffs.size());
        h = Eigen::Map<VectorXp>(h_coeffs.data(), h_coeffs.size());
        soc_dims = Eigen::Map<Eigen::VectorXi>(cone_dimensions.data(), cone_dimensions.size());
    }

    size_t WrapperBase::getNumVariables() const
    {
        return n_variables;
    }

    void WrapperBase::addVariable(Variable &variable)
    {
        if (not variable.isLinkedToProblem())
        {
            variable.linkToProblem(&solution, n_variables);
            n_variables++;
        }
    }

} // namespace op
