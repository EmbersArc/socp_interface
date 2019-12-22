#pragma once

#include <utility>
#include <cstddef>
#include <ostream>
#include <vector>

namespace op
{

// represents an optimization variable x_i
struct Variable
{
    std::string name;
    std::vector<size_t> tensor_indices;
    size_t problem_index;
    friend std::ostream &operator<<(std::ostream &os, const Variable &variable);
};

class VariableMatrix
{
    std::pair<size_t, size_t> dimension;
    std::vector<std::vector<Variable>> variables;
};

} // namespace op