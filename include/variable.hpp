#pragma once

#include <utility>
#include <cstddef>
#include <ostream>
#include <vector>

namespace op
{

// An optimization variable x_i
struct Variable
{
    std::string name;
    std::vector<size_t> tensor_indices;
    size_t problem_index;
    friend std::ostream &operator<<(std::ostream &os, const Variable &variable);
};

struct VariableMatrix
{
    size_t rows() const;
    size_t cols() const;
    std::pair<size_t, size_t> shape() const;
    Variable operator()(const size_t row, const size_t col) const;
    std::vector<std::vector<Variable>> variables;
};

} // namespace op