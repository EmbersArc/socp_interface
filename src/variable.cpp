#include "variable.hpp"

#include <sstream>

namespace op
{

std::ostream& operator<<(std::ostream& os, const Variable& variable)
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