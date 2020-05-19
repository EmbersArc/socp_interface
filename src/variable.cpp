#include "variable.hpp"

#include <sstream>

namespace op
{

    Variable::Variable(const std::string &name, size_t row, size_t col)
        : name(name), index(row, col) {}

    bool Variable::operator==(const Variable &other) const
    {
        return this->name == other.name and this->index == other.index;
    }

    bool Variable::hasProblemIndex() const
    {
        return problem_index.has_value();
    }

    size_t Variable::getProblemIndex() const
    {
        return problem_index.value();
    }

    std::ostream &operator<<(std::ostream &os, const Variable &variable)
    {
        os << variable.name
           << "[" << variable.index.first << ", " << variable.index.second << "]"
           << "@(" << variable.problem_index.value() << ")";
        return os;
    }

} // namespace op
