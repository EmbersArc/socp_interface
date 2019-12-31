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

Variable GenericOptimizationProblem::getVariable(const std::string &name) const
{
    return variables.at(name);
}

size_t GenericOptimizationProblem::getNumVariables() const
{
    return solution_vector.size();
}

void GenericOptimizationProblem::readSolution(const std::string &name,
                                              double &solution) const
{
    const Variable &variable = variables.at(name);
    assert(variable.is_scalar());
    solution = solution_vector[variable.coeff(0).getProblemIndex()];
}
void GenericOptimizationProblem::readSolution(const std::string &name,
                                              DynamicMatrix<double> &solution) const
{
    const Variable &variable = variables.at(name);
    solution.resize(variable.rows(), variable.cols());
    for (size_t row = 0; row < variable.rows(); row++)
    {
        for (size_t col = 0; col < variable.cols(); col++)
        {
            solution.coeffRef(row, col) = solution_vector[variable.coeff(row, col).getProblemIndex()];
        }
    }
}

} // end namespace op
