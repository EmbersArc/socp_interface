#include "variable.hpp"

namespace op
{
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

Variable VariableMatrix::operator()(const size_t row, const size_t col) const
{
    return variables[row][col];
}

std::ostream &operator<<(std::ostream &os, const Variable &variable)
{
    os << variable.name;
    if (variable.tensor_indices.size() > 0)
    {
        os << "[";
        for (size_t i = 0; i < variable.tensor_indices.size(); i++)
        {
            if (i)
                os << ",";
            os << variable.tensor_indices[i];
        }
        os << "]";
    }
    os << "@" << variable.problem_index;
    return os;
}

} // namespace op