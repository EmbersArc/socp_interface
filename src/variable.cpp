#include "variable.hpp"

#include <sstream>

namespace op
{

std::string Variable::print() const
{
    std::ostringstream s;
    s << name;
    if (tensor_indices.size() > 0)
    {
        s << "[";
        for (size_t i = 0; i < tensor_indices.size(); i++)
        {
            if (i)
                s << ",";
            s << tensor_indices[i];
        }
        s << "]";
    }
    s << "@" << problem_index;
    return s.str();
}

} // namespace op