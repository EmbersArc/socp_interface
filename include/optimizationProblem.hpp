#pragma once

#include "constraint.hpp"

#include <map>
#include <vector>

namespace op
{

class GenericOptimizationProblem
{
public:
    VariableMatrix createTensorVariable(const std::string &name, const std::vector<size_t> &dimensions = {});
    size_t getNumVariables() const;
    void readSolution(const std::string &name,
                      std::vector<std::vector<double>> &solution);

#ifdef EIGEN_AVAILABLE
    template <typename Derived>
    void readSolution(const std::string &name,
                      Eigen::PlainObjectBase<Derived> &solution);
#endif

protected:
    std::vector<size_t> solution_vector;
    std::map<std::string, VariableMatrix> variables;
};

#ifdef EIGEN_AVAILABLE
template <typename Derived>
void GenericOptimizationProblem::readSolution(const std::string &name,
                                              Eigen::PlainObjectBase<Derived> &solution)
{
    const VariableMatrix &variable = variables[name];
    for (size_t row = 0; row < variable.rows(); row++)
    {
        for (size_t col = 0; col < variable.cols(); col++)
        {
            solution.coeffRef(row, col) = solution_vector[variable(row, col).problem_index];
        }
    }
}
#endif

} // namespace op