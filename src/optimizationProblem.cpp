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
                                              std::vector<std::vector<double>> &solution)
{
    const Variable &variable = variables[name];
    readSolution(variable, solution);
}

void GenericOptimizationProblem::readSolution(const Variable &variable,
                                              std::vector<std::vector<double>> &solution)
{
    solution.clear();
    for (size_t row = 0; row < variable.rows(); row++)
    {
        std::vector<double> solution_row;
        for (size_t col = 0; col < variable.cols(); col++)
        {
            solution_row.push_back(solution_vector[variable(row, col).getProblemIndex()]);
        }
        solution.push_back(solution_row);
    }
}

} // end namespace op
