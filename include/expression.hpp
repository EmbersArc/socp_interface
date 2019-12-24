#pragma once

#include "parameter.hpp"
#include "variable.hpp"

#include <optional>

namespace op
{

// A linear term (p_1*x_1) or constant term (p_1)
struct AffineTerm
{
    AffineTerm(const Parameter &parameter);
    AffineTerm(const Variable &variable);
    AffineTerm(const Parameter &parameter, const Variable &variable);

    Parameter parameter = Parameter(0.0);
    std::optional<Variable> variable; // a missing Variable represents a constant 1.0
    friend std::ostream &operator<<(std::ostream &os, const AffineTerm &term);
    double evaluate(const std::vector<double> &soln_values) const;
};

// A term like (p_1*x_1 + p_2*x_2 + ... + b)
struct AffineExpression
{
    AffineExpression() = default;
    AffineExpression(const Parameter &parameter);
    AffineExpression(const AffineTerm &parameter);

    std::vector<AffineTerm> terms;
    friend std::ostream &operator<<(std::ostream &os, const AffineExpression &expression);
    double evaluate(const std::vector<double> &soln_values) const;
};

// A 2D matrix of affine expressions
struct AffineMatrix
{
    AffineMatrix() = default;
    explicit AffineMatrix(const AffineExpression &expression);
    size_t rows() const;
    size_t cols() const;
    std::pair<size_t, size_t> shape() const;
    AffineExpression operator()(size_t row, size_t col) const;

    std::vector<std::vector<AffineExpression>> expressions;
};

// A term like norm2([p_1*x_1 + p_2*x_2 + ... + b_1,   p_3*x_3 + p_4*x_4 + ... + b_2 ])
struct Norm2
{
    explicit Norm2(const AffineMatrix &affineMatrix);
    std::vector<AffineExpression> arguments;
    friend std::ostream &operator<<(std::ostream &os, const Norm2 &norm2);
    double evaluate(const std::vector<double> &soln_values) const;
};

AffineTerm operator*(const Parameter &parameter, const Variable &variable);
AffineTerm operator*(const Variable &variable, const Parameter &parameter);

AffineTerm operator*(const double &const_parameter, const Variable &variable);
AffineTerm operator*(const Variable &variable, const double &const_parameter);

AffineExpression operator+(const AffineExpression &lhs, const AffineExpression &rhs);

AffineExpression operator+(const AffineExpression &lhs, const double &rhs);
AffineExpression operator+(const double &lhs, const AffineExpression &rhs);

AffineMatrix operator+(const AffineMatrix &lhs, const AffineMatrix &rhs);
AffineMatrix operator*(const ParameterMatrix &parameter, const VariableMatrix &variable);
AffineMatrix operator+(const ParameterMatrix &parameter, const VariableMatrix &variable);

AffineMatrix operator+(const ParameterMatrix &parameter, const AffineMatrix &affine);

} // namespace op