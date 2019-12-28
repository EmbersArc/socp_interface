#pragma once

#include "constraint.hpp"

#include <map>
#include <vector>

namespace op
{

class GenericOptimizationProblem
{
public:
    Variable createVariable(const std::string &name,
                            size_t rows = 1, size_t cols = 1);

    size_t getNumVariables() const;

    void readSolution(const std::string &name,
                      DynamicMatrix<double> &solution);
    void readSolution(const Variable &variable,
                      DynamicMatrix<double> &solution);

#ifdef EIGEN_AVAILABLE
    template <typename Derived>
    void readSolution(const Variable &variable,
                      Eigen::PlainObjectBase<Derived> &solution);
    template <typename Derived>
    void readSolution(const std::string &name,
                      Eigen::PlainObjectBase<Derived> &solution);
#endif

    std::vector<double> solution_vector;

protected:
    std::map<std::string, Variable> variables;
};

#ifdef EIGEN_AVAILABLE
template <typename Derived>
void GenericOptimizationProblem::readSolution(const Variable &variable,
                                              Eigen::PlainObjectBase<Derived> &solution)
{
    solution.resize(variable.rows(), variable.cols());
    for (size_t row = 0; row < variable.rows(); row++)
    {
        for (size_t col = 0; col < variable.cols(); col++)
        {
            solution.coeffRef(row, col) = solution_vector[variable(row, col).getProblemIndex()];
        }
    }
}

template <typename Derived>
void GenericOptimizationProblem::readSolution(const std::string &name,
                                              Eigen::PlainObjectBase<Derived> &solution)
{
    const Variable &variable = variables[name];
    readSolution(variable, solution);
}
#endif

} // namespace op