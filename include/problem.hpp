#pragma once

#include "constraint.hpp"
#include <Eigen/Sparse>

namespace op
{

    class OptimizationProblem
    {
    public:
        void addConstraint(std::vector<Constraint> constraints);

        void addMinimizationTerm(const Expression &term);

        bool isFeasible() const;

        void finalize();

        friend std::ostream &operator<<(std::ostream &os, const OptimizationProblem &socp);

    private:
        using MatrixXp = Eigen::Matrix<Parameter, Eigen::Dynamic, Eigen::Dynamic>;
        using VectorXp = Eigen::Matrix<Parameter, Eigen::Dynamic, 1>;

        Expression costFunction;
        std::vector<EqualityConstraint> equality_constraints;
        std::vector<PositiveConstraint> positive_constraints;
        std::vector<SecondOrderConeConstraint> second_order_cone_constraints;
        std::vector<double> solution;
        std::vector<Eigen::Triplet<Parameter>> A_coeffs, G_coeffs;
        std::vector<Parameter> b_coeffs, h_coeffs;

        Eigen::SparseMatrix<Parameter> A, G;
        VectorXp c, b, h;

        void addVariable(Variable &variable);

        size_t n_variables;

        bool is_socp;
        bool is_quadratic;
    };

} // namespace op