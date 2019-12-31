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
    explicit AffineTerm(const ParameterSource &parameter);
    explicit AffineTerm(const VariableSource &variable);
    AffineTerm(const ParameterSource &parameter,
               const VariableSource &variable);

    operator AffineSum() const;

    ParameterSource parameter;
    std::optional<VariableSource> variable; // a missing Variable represents a constant 1.0
    friend std::ostream &operator<<(std::ostream &os, const AffineTerm &term);
    double evaluate(const std::vector<double> &soln_values) const;
};
AffineTerm operator*(const ParameterSource &parameter, const VariableSource &variable);
AffineTerm operator*(const double &const_parameter, const VariableSource &variable);

// A term like (p_1*x_1 + p_2*x_2 + ... + b)
struct AffineSum
{
    AffineSum() = default;
    explicit AffineSum(const ParameterSource &parameter);
    explicit AffineSum(const VariableSource &variable);
    explicit AffineSum(const AffineTerm &term);

    std::vector<AffineTerm> terms;
    friend std::ostream &operator<<(std::ostream &os, const AffineSum &expression);
    double evaluate(const std::vector<double> &soln_values) const;
    AffineSum &operator+=(const AffineSum &other);
};
AffineSum operator+(const AffineSum &lhs, const AffineSum &rhs);
AffineSum operator+(const double &lhs, const AffineSum &rhs);
AffineSum operator+(const AffineSum &lhs, const double &rhs);

// A scalar/vector/matrix of affine expressions
class AffineExpression : public DynamicMatrix<AffineSum>
{
public:
    using DynamicMatrix<AffineSum>::DynamicMatrix;
    AffineExpression() = default;
    explicit AffineExpression(const Parameter &parameter);
    explicit AffineExpression(const AffineSum &expression);
    friend std::ostream &operator<<(std::ostream &os, const AffineExpression &expression);
};
AffineExpression operator+(const AffineExpression &lhs, const AffineExpression &rhs);
AffineExpression operator*(const Parameter &parameter, const Variable &variable);
AffineExpression operator*(const Variable &variable, const Parameter &parameter);
AffineExpression operator*(const double &const_parameter, const Variable &variable);

// A term like norm2([p_1*x_1 + p_2*x_2 + ... + b_1,   p_3*x_3 + p_4*x_4 + ... + b_2 ])
struct Norm2
{
    explicit Norm2(const AffineExpression &affine);
    std::vector<AffineSum> arguments;
    friend std::ostream &operator<<(std::ostream &os, const Norm2 &norm2);
    double evaluate(const std::vector<double> &soln_values) const;
};

} // namespace op