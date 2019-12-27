#pragma once

#include "parameter.hpp"
#include "variable.hpp"

#include <optional>

namespace op
{

// A linear term (p_1*x_1) or constant term (p_1)
struct AffineTerm
{
    AffineTerm();
    AffineTerm(const ParameterSource &parameter);
    AffineTerm(const VariableSource &variable);
    AffineTerm(const ParameterSource &parameter,
               const VariableSource &variable);

    ParameterSource parameter;
    std::optional<VariableSource> variable; // a missing Variable represents a constant 1.0
    friend std::ostream &operator<<(std::ostream &os, const AffineTerm &term);
    double evaluate(const std::vector<double> &soln_values) const;
};
AffineTerm operator*(const ParameterSource &parameter, const VariableSource &variable);
AffineTerm operator*(const double &const_parameter, const VariableSource &variable);

// A term like (p_1*x_1 + p_2*x_2 + ... + b)
struct AffineExpression
{
    AffineExpression() = default;
    AffineExpression(const ParameterSource &parameter);
    AffineExpression(const AffineTerm &parameter);

    std::vector<AffineTerm> terms;
    friend std::ostream &operator<<(std::ostream &os, const AffineExpression &expression);
    double evaluate(const std::vector<double> &soln_values) const;
};
AffineExpression operator+(const AffineExpression &lhs, const AffineExpression &rhs);
AffineExpression operator+(const double &lhs, const AffineExpression &rhs);
AffineExpression operator+(const AffineExpression &lhs, const double &rhs);

// A scalar/vector/matrix of affine expressions
struct Affine
{
    Affine() = default;
    explicit Affine(const AffineExpression &expression);
    size_t rows() const;
    size_t cols() const;
    std::pair<size_t, size_t> shape() const;
    AffineExpression operator()(size_t row, size_t col) const;
    std::vector<std::vector<AffineExpression>> expressions;
};
Affine operator+(const Affine &lhs, const Affine &rhs);
Affine operator*(const Parameter &parameter, const Variable &variable);
Affine operator*(const double &const_parameter, const Variable &variable);

// A term like norm2([p_1*x_1 + p_2*x_2 + ... + b_1,   p_3*x_3 + p_4*x_4 + ... + b_2 ])
struct Norm2
{
    explicit Norm2(const Affine &affine);
    std::vector<AffineExpression> arguments;
    friend std::ostream &operator<<(std::ostream &os, const Norm2 &norm2);
    double evaluate(const std::vector<double> &soln_values) const;
};

} // namespace op