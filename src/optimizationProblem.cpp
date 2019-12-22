#include "optimizationProblem.hpp"

#include <cassert>
#include <numeric>

namespace op
{

size_t allocateVariableIndex();

void GenericOptimizationProblem::createTensorVariable(const std::string &name,
                                                      const std::vector<size_t> &dimensions)
{
    variables.insert({name, VariableMatrix(name, solution_vector.size(), {0, 0})});
}

size_t GenericOptimizationProblem::getNumVariables() const
{
    return solution_vector.size();
}

void GenericOptimizationProblem::readSolution(const std::string &name,
                                              std::vector<std::vector<double>> &solution)
{
    const VariableMatrix &variable = variables[name];
    solution.clear();
    for (size_t row = 0; row < variable.rows(); row++)
    {
        std::vector<double> solution_row;
        for (size_t col = 0; col < variable.cols(); col++)
        {
            solution_row.push_back(solution_vector[variable(row, col).problem_index]);
        }
        solution.push_back(solution_row);
    }
}

} // end namespace op
