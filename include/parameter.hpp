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
};

class ParameterMatrix;

// Represents a parameter p_i in the opt-problem that can be changed between problem evaluations.
// The parameter value can either be constant or dynamically accessed through a pointer or callback.
class Parameter
{
    using parameter_variant_t = std::variant<double,
                                             const double *,
                                             std::function<double()>>;
    parameter_variant_t source;
    ParameterSourceType sourceType;

public:
    Parameter();
    Parameter(const double const_value);
    explicit Parameter(const double *dynamic_value_ptr);
    explicit Parameter(std::function<double()> callback);
    ~Parameter();

    double get_value() const;

    friend std::ostream &operator<<(std::ostream &os, const Parameter &parameter);

    Parameter operator+(const Parameter &other) const;
    Parameter operator-(const Parameter &other) const;
    Parameter operator*(const Parameter &other) const;
    Parameter operator/(const Parameter &other) const;
    Parameter operator-() const;
    ParameterMatrix operator*(const ParameterMatrix &other) const;
};

class ParameterMatrix
{
public:
    explicit ParameterMatrix(const std::vector<std::vector<Parameter>> &matrix);
    size_t rows() const;
    size_t cols() const;
    std::pair<size_t, size_t> shape() const;
    Parameter operator()(const size_t row, const size_t col) const;
    ParameterMatrix operator+(const ParameterMatrix &other) const;
    ParameterMatrix operator-(const ParameterMatrix &other) const;
    ParameterMatrix operator*(const ParameterMatrix &other) const;
    ParameterMatrix operator*(const Parameter &other) const;
    ParameterMatrix operator/(const Parameter &other) const;

private:
    std::vector<std::vector<Parameter>> matrix;
};

} // namespace op