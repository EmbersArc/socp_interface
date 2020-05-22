#pragma once

#include <utility>
#include <ostream>
#include <vector>
#include <string>
#include <optional>

namespace op
{
    struct Term;
    class Expression;

    class Variable
    {
    public:
        Variable() = default;
        Variable(const std::string &name, size_t row, size_t col);

        bool operator==(const Variable &other) const;

        size_t getProblemIndex() const;
        bool hasProblemIndex() const;

        friend std::ostream &operator<<(std::ostream &os,
                                        const Variable &variable);
        operator Term() const;
        operator Expression() const;
        std::string name;

    private:
        std::optional<size_t> problem_index;
        std::pair<size_t, size_t> index;
    };

} // namespace op