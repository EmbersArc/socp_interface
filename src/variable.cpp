#include "variable.hpp"

#include <sstream>

namespace op
{

    Variable::Variable(const std::string &name, size_t row, size_t col)
        : name(name), index(row, col)
    {
        this->solution_reference = std::make_shared<solution_reference_t>(nullptr, 0);
    }

    bool Variable::operator==(const Variable &other) const
    {
        return this->name == other.name and
               this->index == other.index and
               this->solution_reference == other.solution_reference;
    }

    bool Variable::isLinkedToProblem() const
    {
        return solution_reference->first != nullptr;
    }

    void Variable::linkToProblem(std::vector<double> *solution_ptr, size_t problem_index)
    {
        this->solution_reference->first = solution_ptr;
        this->solution_reference->second = problem_index;
    }

    double Variable::getSolution() const
    {
        if (not this->isLinkedToProblem())
        {
            // Don't throw error here since variables might indeed be unused.
            return 0.;
        }
        else
        {
            return this->solution_reference->first->at(this->solution_reference->second);
        }
    }

    size_t Variable::getProblemIndex() const
    {
        return this->solution_reference->second;
    }

    std::ostream &
    operator<<(std::ostream &os, const Variable &variable)
    {
        os << variable.name
           << "[" << variable.index.first << ", " << variable.index.second << "]";

        if (variable.isLinkedToProblem())
            os << "@(" << variable.solution_reference->second << ")";

        return os;
    }

} // namespace op
