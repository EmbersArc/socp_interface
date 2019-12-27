#include "variable.hpp"

#include <sstream>

namespace op
{

VariableSource::VariableSource(const std::string &name, size_t problem_index,
                               size_t row, size_t col)
    : name(name), problem_index(problem_index), index(row, col) {}

size_t VariableSource::getProblemIndex() const
{
    return problem_index;
}

std::ostream &operator<<(std::ostream &os, const VariableSource &variable)
{
    os << variable.name
       << "[" << variable.index.first << ", " << variable.index.second << "]"
       << "@(" << variable.problem_index << ")";
    return os;
}

Variable::Variable(const std::string &name, size_t start_index,
                   size_t rows, size_t cols)
    : name(name)
{
    size_t index = start_index;
    for (size_t row = 0; row < rows; row++)
    {
        std::vector<VariableSource> row_variables;
        for (size_t col = 0; col < cols; col++)
        {
            row_variables.emplace_back(name, index, row, col);
            index++;
        }
        variables.push_back(row_variables);
    }
}

size_t Variable::rows() const
{
    return variables.size();
}

size_t Variable::cols() const
{
    return variables.front().size();
}
std::pair<size_t, size_t> Variable::shape() const
{
    return {rows(), cols()};
}

VariableSource Variable::operator()(size_t row, size_t col) const
{
    return variables[row][col];
}

std::ostream &operator<<(std::ostream &os, const Variable &variable)
{
    os << variable.name
       << "(" << variable.rows() << "x" << variable.cols() << ")";
    return os;
}

} // namespace op