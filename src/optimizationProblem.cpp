#include "optimizationProblem.hpp"

#include <cassert>
#include <numeric>

namespace op
{

size_t allocateVariableIndex();

VariableMatrix GenericOptimizationProblem::createTensorVariable(const std::string &name,
                                                                const std::vector<size_t> &dimensions)
{
    assert(dimensions.size() <= 2);
    std::pair<size_t, size_t> shape;
    if (dimensions.empty())
        shape = {1, 1};
    if (dimensions.size() == 1)
        shape = {dimensions[0], 1};
    if (dimensions.size() == 2)
        shape = {dimensions[0], dimensions[1]};

    VariableMatrix variableMatrix(name, solution_vector.size(), shape);
    variables.insert({name, variableMatrix});
    solution_vector.resize(solution_vector.size() + shape.first * shape.second);

    return variableMatrix;
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
