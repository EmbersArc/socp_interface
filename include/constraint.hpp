#pragma once

#include "expression.hpp"

#include <vector>
#include <ostream>

namespace op
{

// represents a constraint like
//      norm2([p_1*x_1 + p_2*x_2 + ... + b_1,   p_3*x_3 + p_4*x_4 + ... + b_2 ])
//        <= p_5*x_5 + p_6*x_6 + ... + b_3
struct SecondOrderConeConstraint
{
    Norm2 lhs;
    AffineExpression rhs;
    friend std::ostream &operator<<(std::ostream &os, const SecondOrderConeConstraint &constraint);
    double evaluate(const std::vector<double> &soln_values) const;
};

// represents a constraint like
//     p_1*x_1 + p_2*x_2 + ... + b >= 0
struct PostiveConstraint
{
    AffineExpression lhs;
    friend std::ostream &operator<<(std::ostream &os, const PostiveConstraint &constraint);
    double evaluate(const std::vector<double> &soln_values) const;
};

// represents a constraint like
//     p_1*x_1 + p_2*x_2 + ... + b == 0
struct EqualityConstraint
{
    AffineExpression lhs;
    friend std::ostream &operator<<(std::ostream &os, const EqualityConstraint &constraint);
    double evaluate(const std::vector<double> &soln_values) const;
};

SecondOrderConeConstraint operator<=(const Norm2 &lhs, const AffineExpression &rhs);
SecondOrderConeConstraint operator>=(const AffineExpression &lhs, const Norm2 &rhs);
SecondOrderConeConstraint operator<=(const Norm2 &lhs, const double &rhs);
SecondOrderConeConstraint operator>=(const double &lhs, const Norm2 &rhs);
PostiveConstraint operator>=(const AffineExpression &lhs, const double &zero);
PostiveConstraint operator<=(const double &zero, const AffineExpression &rhs);
EqualityConstraint operator==(const AffineExpression &lhs, const double &zero);

} // namespace op