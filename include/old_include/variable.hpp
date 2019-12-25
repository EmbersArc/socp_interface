#pragma once

#include <utility>
#include <cstddef>
#include <ostream>
#include <vector>

namespace op
{

// A scalar/vector/matrix optimization variable
class Variable
{
    Variable(const std::string &name, size_t start_index,
             size_t rows = 1, size_t cols = 1);
    friend std::ostream &operator<<(std::ostream &os, const Variable &variable);

public:
    size_t rows() const;
    size_t cols() const;
    std::pair<size_t, size_t> shape() const;
    size_t operator()(size_t row, size_t col) const;
    friend std::ostream &operator<<(std::ostream &os, const Variable &variable);

private:
    // indices of the value in the solution vector x
    std::vector<std::vector<size_t>> indices;
    std::string name;
};

} // namespace op