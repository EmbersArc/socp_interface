#include "optimizationProblem.hpp"

#include <cassert>
#include <numeric>

namespace op
{

size_t allocateVariableIndex();

VariableMatrix GenericOptimizationProblem::createVariableMatrix(const std::string &name,
                                                                size_t rows, size_t cols)
{
    VariableMatrix variableMatrix(name, solution_vector.size(), {rows, cols});
    variables.insert({name, variableMatrix});
    solution_vector.resize(solution_vector.size() + rows * cols);

    return variableMatrix;
}

Variable GenericOptimizationProblem::createVariable(const std::string &name)
{
    Variable variable(name, solution_vector.size());
    variables.insert({name, VariableMatrix(variable)});
    solution_vector.resize(solution_vector.size() + 1);

    return variable;
}

size_t GenericOptimizationProblem::getNumVariables() const
{
    return solution_vector.size();
}

void GenericOptimizationProblem::readSolution(const std::string &name,
                                              std::vector<std::vector<double>> &solution)
{
    const VariableMatrix &variable = variables[name];
    readSolution(variable, solution);
}

void GenericOptimizationProblem::readSolution(const std::string &name,
                                              double &solution)
{
    const VariableMatrix &variable = variables[name];
    assert(variable.rows() == 1 and variable.cols() == 1);
    readSolution(variable(0, 0), solution);
}

void GenericOptimizationProblem::readSolution(const VariableMatrix &variable,
                                              std::vector<std::vector<double>> &solution)
{
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

void GenericOptimizationProblem::readSolution(const Variable &variable,
                                              double &solution)
{
    solution = solution_vector[variable.problem_index];
}

} // end namespace op
