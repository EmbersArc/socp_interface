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
    friend std::ostream& operator<<(std::ostream& os, const Variable& variable);
};

} // namespace op