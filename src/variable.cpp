#include "variable.hpp"

#include <sstream>

namespace op
{

Variable::Variable(const std::string &name, size_t problem_index)
    : name(name), problem_index(problem_index) {}

VariableMatrix::VariableMatrix(const Variable &variable)
{
    variables.resize(1);
    variables.front().push_back(variable);
}

VariableMatrix::VariableMatrix(const std::string &name, size_t start_index,
                               const std::pair<size_t, size_t> shape) : name(name)
{
    size_t index = start_index;
    for (size_t row = 0; row < shape.first; row++)
    {
        std::vector<Variable> variables_row;
        for (size_t col = 0; col < shape.second; col++)
        {
            std::stringstream variable_name;
            variable_name << name << "[" << row << "," << col << "]";
            variables_row.emplace_back(variable_name.str(), index);
            index++;
        }
        variables.push_back(variables_row);
    }
}

size_t VariableMatrix::rows() const
{
    return variables.size();
}

size_t VariableMatrix::cols() const
{
    return variables.front().size();
}

std::pair<size_t, size_t> VariableMatrix::shape() const
{
    return {rows(), cols()};
}

Variable VariableMatrix::operator()(size_t row, size_t col) const
{
    return variables[row][col];
}

std::ostream &operator<<(std::ostream &os, const VariableMatrix &variable)
{
    os << variable.name;
    return os;
}

std::ostream &operator<<(std::ostream &os, const Variable &variable)
{
    os << variable.name << "@" << variable.problem_index;
    return os;
}

} // namespace op