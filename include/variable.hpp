#pragma once

#include <utility>
#include <cstddef>

namespace op
{

// represents an optimization variable x_i
class Variable
{
};

class VariableVector : public Variable
{
    size_t dimension;
};

class VariableMatrix : public Variable
{
    std::pair<size_t, size_t> dimension;
};

} // namespace op