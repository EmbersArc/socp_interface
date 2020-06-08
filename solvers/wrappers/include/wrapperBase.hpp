#pragma once

#include "problem.hpp"

namespace op
{

    class WrapperBase
    {

    public:
        explicit WrapperBase(OptimizationProblem &problem);
        virtual bool solveProblem(bool verbose = false) = 0;
        virtual std::string getResultString() const = 0;
        virtual void initialize() = 0;

    protected:
        using MatrixXp = Eigen::Matrix<Parameter, Eigen::Dynamic, Eigen::Dynamic>;
        using VectorXp = Eigen::Matrix<Parameter, Eigen::Dynamic, 1>;

        size_t n_variables = 0;
        Eigen::VectorXi soc_dims;
        Eigen::SparseMatrix<Parameter> A;
        Eigen::SparseMatrix<Parameter> G;
        VectorXp c;
        VectorXp h;
        VectorXp b;

        std::vector<double> solution;

        size_t getNumVariables() const;
        void addVariable(Variable &variable);
    };

} // namespace op