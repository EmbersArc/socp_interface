#include "optimizationProblem.hpp"

#include <cassert>
#include <numeric>

namespace op
{

size_t allocateVariableIndex();

Variable GenericOptimizationProblem::createVariable(const std::string &name,
                                                    size_t rows, size_t cols)
{
    Variable variable(name, solution_vector.size(), rows, cols);
    variables.insert({name, variable});
    solution_vector.resize(solution_vector.size() + rows * cols);

    return variable;
}

size_t GenericOptimizationProblem::getNumVariables() const
{
    return solution_vector.size();
}

void GenericOptimizationProblem::readSolution(const std::string &name,
                                              DynamicMatrix<double> &solution) const
{
    readSolution(variables.at(name), solution);
}

void GenericOptimizationProblem::readSolution(const Variable &variable,
                                              DynamicMatrix<double> &solution) const
{
    solution.resize(variable.rows(), variable.cols());
    for (size_t row = 0; row < variable.rows(); row++)
    {
        for (size_t col = 0; col < variable.cols(); col++)
        {
            solution(row, col) = solution_vector[variable(row, col).getProblemIndex()];
        }
    }
}

} // end namespace op
