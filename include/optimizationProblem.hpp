#pragma once

#include "constraint.hpp"

#include <map>
#include <vector>

namespace op
{

class GenericOptimizationProblem
{
public:
    Variable createVariable(const std::string &name);

    VariableMatrix createVariableMatrix(const std::string &name,
                                        size_t rows, size_t cols = 1);

    size_t getNumVariables() const;

    void readSolution(const std::string &name,
                      double &solution);
    void readSolution(const std::string &name,
                      std::vector<std::vector<double>> &solution);
    void readSolution(const Variable &variable,
                      double &solution);
    void readSolution(const VariableMatrix &variable,
                      std::vector<std::vector<double>> &solution);

#ifdef EIGEN_AVAILABLE
    template <typename Derived>
    void readSolution(const VariableMatrix &variable,
                      Eigen::PlainObjectBase<Derived> &solution);
    template <typename Derived>
    void readSolution(const std::string &name,
                      Eigen::PlainObjectBase<Derived> &solution);
#endif

    std::vector<double> solution_vector;

protected:
    std::map<std::string, VariableMatrix> variables;
};

#ifdef EIGEN_AVAILABLE
template <typename Derived>
void GenericOptimizationProblem::readSolution(const VariableMatrix &variable,
                                              Eigen::PlainObjectBase<Derived> &solution)
{
    for (size_t row = 0; row < variable.rows(); row++)
    {
        for (size_t col = 0; col < variable.cols(); col++)
        {
            solution.coeffRef(row, col) = solution_vector[variable(row, col).problem_index];
        }
    }
}

template <typename Derived>
void GenericOptimizationProblem::readSolution(const std::string &name,
                                              Eigen::PlainObjectBase<Derived> &solution)
{
    const VariableMatrix &variable = variables[name];
    readSolution(variable, solution);
}
#endif

} // namespace op