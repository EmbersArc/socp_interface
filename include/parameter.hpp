#pragma once

#include <functional>
#include <string>
#include <utility>
#include <variant>
#include <memory>

namespace op
{

// Represents a parameter p_i in the opt-problem that can be changed between problem evaluations.
// The parameter value can either be constant or dynamically accessed through a pointer or callback.
class Parameter
{
    using parameter_variant_t = std::variant<double,
                                             const double *,
                                             std::function<double()>>;
    parameter_variant_t source;

public:
    Parameter();
    Parameter(double const_value);
    explicit Parameter(const double *dynamic_value_ptr);
    explicit Parameter(std::function<double()> callback);
    ~Parameter();

    double get_value() const;

    friend std::ostream &operator<<(std::ostream &os, const Parameter &parameter);

    Parameter operator+(const Parameter &par);
    Parameter operator-(const Parameter &par);
    Parameter operator-();
    Parameter operator*(const Parameter &par);
    Parameter operator/(const Parameter &par);
};

} // namespace op