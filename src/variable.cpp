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

    bool Variable::isLinkedToProblem() const
    {
        return solution_reference != nullptr;
    }

    void Variable::linkToProblem(double *solution_ptr, size_t problem_index)
    {
        this->solution_reference = std::make_shared<solution_reference_t>(solution_ptr, problem_index);
    }

    double Variable::getSolution() const
    {
        if (not this->isLinkedToProblem())
        {
            // Cannot throw error here since parts of a matrix might indeed be unused.
            return 0.;
        }
        else
        {
            return this->solution_reference->first[this->solution_reference->second];
        }
    }

    std::ostream &operator<<(std::ostream &os, const Variable &variable)
    {
        os << variable.name
           << "[" << variable.index.first << ", " << variable.index.second << "]";

        if (variable.isLinkedToProblem())
            os << "@(" << variable.solution_reference->second << ")";

        return os;
    }

} // namespace op
