#pragma once

#include <utility>
#include <ostream>
#include <vector>
#include <string>
#include <memory>

namespace op
{
    class Term;
    class Expression;

    class Variable
    {
    public:
        Variable() = default;
        Variable(const std::string &name, size_t row, size_t col);

        bool operator==(const Variable &other) const;

        bool isLinkedToProblem() const;
        void linkToProblem(double *solution_ptr, size_t problem_index);
        double getSolution() const;

        friend std::ostream &operator<<(std::ostream &os,
                                        const Variable &variable);
        operator Term() const;
        operator Expression() const;
        std::string name;

    private:
        using solution_reference_t = std::pair<double *, size_t>;
        std::shared_ptr<solution_reference_t> solution_reference;
        std::pair<size_t, size_t> index;
    };

} // namespace op