#pragma once

#include <utility>
#include <cstddef>
#include <ostream>
#include <vector>
#include <string>

namespace op
{

class VariableSource
{
public:
    VariableSource(const std::string &name, size_t problem_index,
                   size_t row, size_t col);
    size_t getProblemIndex();
    friend std::ostream &operator<<(std::ostream &os, const VariableSource &variable);

private:
    std::string name;
    size_t problem_index;
    std::pair<size_t, size_t> index;
};

// A scalar/vector/matrix optimization variable
class Variable
{
public:
    Variable(const std::string &name, size_t start_index,
             size_t rows = 1, size_t cols = 1);

    size_t rows() const;
    size_t cols() const;
    std::pair<size_t, size_t> shape() const;
    size_t operator()(size_t row, size_t col) const;
    friend std::ostream &operator<<(std::ostream &os, const Variable &variable);

private:
    // indices of the value in the solution vector x
    std::vector<std::vector<VariableSource>> variables;
    std::string name;
};

} // namespace op