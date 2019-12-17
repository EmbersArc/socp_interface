#pragma once

#include <functional>
#include <string>
#include <variant>

namespace op
{

enum class ParameterSourceType
{
    Constant,
    Pointer,
    Callback
};

// Represents a parameter p_i in the opt-problem that can be changed between problem evaluations.
// The parameter value can either be constant or dynamically accessed through a pointer or callback.
class Parameter
{
    std::variant<std::function<double()>,
                 const double *,
                 double>
        source;

    ParameterSourceType sourceType;

public:
    Parameter();

    explicit Parameter(std::function<double()> callback);

    explicit Parameter(const double *dynamic_value_ptr);

    explicit Parameter(double const_value);

    double get_value() const;

    friend std::ostream& operator<<(std::ostream& os, const Parameter& parameter);
};

} // namespace op