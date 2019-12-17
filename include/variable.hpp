#pragma once

#include <string>
#include <vector>

namespace op
{

// represents an optimization variable x_i
struct Variable
{
    std::string name;
    std::vector<size_t> tensor_indices;
    size_t problem_index;
    std::string print() const;
};

} // namespace op