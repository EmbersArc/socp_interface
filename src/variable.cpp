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
    resize(rows, cols);

    size_t index = start_index;
    for (size_t row = 0; row < rows; row++)
    {
        for (size_t col = 0; col < cols; col++)
        {
            data_matrix[row][col] = VariableSource(name, index, row, col);
            index++;
        }
    }
}

Variable Variable::row(size_t index) const
{
    Variable row_variable(1, cols());
    for (size_t col = 0; col < cols(); col++)
    {
        row_variable.coeffRef(0, col) = coeff(index, col);
    }
    return row_variable;
}

Variable Variable::col(size_t index) const
{
    Variable col_variable(rows(), 1);
    for (size_t row = 0; row < rows(); row++)
    {
        col_variable.coeffRef(row, 0) = coeff(row, index);
    }
    return col_variable;
}

Variable Variable::transpose() const
{
    Variable transposed(cols(), rows());
    for (size_t row = 0; row < rows(); row++)
    {
        for (size_t col = 0; col < cols(); col++)
        {
            transposed.coeffRef(col, row) = coeff(row, col);
        }
    }
    return transposed;
}

std::ostream &operator<<(std::ostream &os, const Variable &variable)
{
    os << variable.name
       << "(" << variable.rows() << "x" << variable.cols() << ")";
    return os;
}

} // namespace op