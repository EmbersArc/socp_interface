#pragma once

#include "parameter.hpp"
#include "variable.hpp"

#include <optional>

namespace op
{

// represents a linear term (p_1*x_1) or constant term (p_1)
struct AffineTerm
{
    AffineTerm() = default;
    AffineTerm(const Parameter &parameter);

    Parameter parameter = Parameter(0.0);
    std::optional<Variable> variable; // a missing Variable represents a constant 1.0
    friend std::ostream &operator<<(std::ostream &os, const AffineTerm &term);
    double evaluate(const std::vector<double> &soln_values) const;
};

// represents a term like (p_1*x_1 + p_2*x_2 + ... + b)
struct AffineExpression
{
    AffineExpression() = default;
    AffineExpression(const Parameter &parameter);
    AffineExpression(const AffineTerm &parameter);

    std::vector<AffineTerm> terms;
    friend std::ostream &operator<<(std::ostream &os, const AffineExpression &expression);
    double evaluate(const std::vector<double> &soln_values) const;
};

struct AffineMatrix
{
    AffineMatrix(const AffineExpression &expression);

    std::vector<std::vector<AffineExpression>> expressions;
};

// represents a term like norm2([p_1*x_1 + p_2*x_2 + ... + b_1,   p_3*x_3 + p_4*x_4 + ... + b_2 ])
struct Norm2
{
    explicit Norm2(const std::vector<AffineExpression> &affineExpressions);
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

} // namespace op