#pragma once

#include <functional>
#include <string>
#include <utility>
#include <variant>
#include <memory>

namespace op
{

enum class ParameterSourceType
{
    Constant,
    Pointer,
    Callback,
    Operation
};

struct ParameterOperation;

// Represents a parameter p_i in the opt-problem that can be changed between problem evaluations.
// The parameter value can either be constant or dynamically accessed through a pointer or callback.
class Parameter
{
    using parameter_variant_t = std::variant<double,
                                             const double *,
                                             std::function<double()>,
                                             std::shared_ptr<ParameterOperation>>;
    parameter_variant_t source;
    ParameterSourceType sourceType;

public:
    Parameter();
    Parameter(double const_value);
    explicit Parameter(const double *dynamic_value_ptr);
    explicit Parameter(std::function<double()> callback);
    explicit Parameter(std::shared_ptr<ParameterOperation> operation);
    ~Parameter();

    double get_value() const;

    friend std::ostream &operator<<(std::ostream &os, const Parameter &parameter);

    friend Parameter operator+(const Parameter &lhs, const Parameter &rhs);
    friend Parameter operator-(const Parameter &lhs, const Parameter &rhs);
    friend Parameter operator-(const Parameter &par);
    friend Parameter operator*(const Parameter &lhs, const Parameter &rhs);
    friend Parameter operator/(const Parameter &lhs, const Parameter &rhs);
};

struct ParameterOperation
{
    ParameterOperation(const Parameter &lhs, const Parameter &rhs,
                       const std::function<double(double, double)> &operation);
    ~ParameterOperation();
    std::pair<const Parameter, const Parameter> terms;
    std::function<double(double, double)> operation;
    double get_value() const;
};

} // namespace op