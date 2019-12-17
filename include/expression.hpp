#pragma once

#include "parameter.hpp"
#include "variable.hpp"

namespace op
{

struct AffineExpression;

// represents a linear term (p_1*x_1) or constant term (p_1)
struct AffineTerm
{
    AffineTerm() = default;
    AffineTerm(const Parameter &parameter);
    operator AffineExpression();

    Parameter parameter = Parameter(0.0);
    std::optional<Variable> variable; // a missing Variable represents a constant 1.0
    std::string print() const;
    double evaluate(const std::vector<double> &soln_values) const;
};

// represents a term like (p_1*x_1 + p_2*x_2 + ... + b)
struct AffineExpression
{
    AffineExpression() = default;
    AffineExpression(const Parameter &parameter);

    std::vector<AffineTerm> terms;
    std::string print() const;
    double evaluate(const std::vector<double> &soln_values) const;
};

// represents a term like norm2([p_1*x_1 + p_2*x_2 + ... + b_1,   p_3*x_3 + p_4*x_4 + ... + b_2 ])
struct Norm2
{
    std::vector<AffineExpression> arguments;
    std::string print() const;
    double evaluate(const std::vector<double> &soln_values) const;
};

Norm2 norm2(const std::vector<AffineExpression> &affineExpressions);

AffineTerm operator*(const Parameter &parameter, const Variable &variable);
AffineTerm operator*(const Variable &variable, const Parameter &parameter);

AffineTerm operator*(const double &const_parameter, const Variable &variable);
AffineTerm operator*(const Variable &variable, const double &const_parameter);

AffineExpression operator+(const AffineExpression &lhs, const AffineExpression &rhs);

AffineExpression operator+(const AffineExpression &lhs, const double &rhs);
AffineExpression operator+(const double &lhs, const AffineExpression &rhs);

} // namespace op