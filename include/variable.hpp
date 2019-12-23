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
    Variable(const std::string &name, size_t problem_index);
    std::string name;
    size_t problem_index;
    friend std::ostream &operator<<(std::ostream &os, const Variable &variable);
};

// A matrix of optimization variables
class VariableMatrix
{
public:
    VariableMatrix() = default;
    VariableMatrix(const Variable &variable);
    VariableMatrix(const std::string &name, size_t start_index,
                   const std::pair<size_t, size_t> shape);

    size_t rows() const;
    size_t cols() const;
    std::pair<size_t, size_t> shape() const;
    Variable operator()(size_t row, size_t col) const;
    friend std::ostream &operator<<(std::ostream &os, const VariableMatrix &variable);

private:
    // indices of the value in the solution vector x
    std::vector<std::vector<Variable>> variables;
    std::string name;
};

} // namespace op