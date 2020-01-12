#pragma once

#include "expression.hpp"

#include <vector>
#include <ostream>

namespace op
{

// represents a constraint like
//     p_1*x_1 + p_2*x_2 + ... + b == 0
struct EqualityConstraint
{
    explicit EqualityConstraint(const AffineSum &affine);
    AffineSum affine;
    friend std::ostream &operator<<(std::ostream &os, const EqualityConstraint &constraint);
    double evaluate(const std::vector<double> &soln_values) const;
};

std::vector<EqualityConstraint> operator==(const Affine &affine, const double &zero);
std::vector<EqualityConstraint> operator==(const Affine &lhs, const Affine &rhs);

// represents a constraint like
//     p_1*x_1 + p_2*x_2 + ... + b >= 0
struct PositiveConstraint
{
    explicit PositiveConstraint(const AffineSum &affine);
    AffineSum affine;
    friend std::ostream &operator<<(std::ostream &os, const PositiveConstraint &constraint);
    double evaluate(const std::vector<double> &soln_values) const;
};

std::vector<PositiveConstraint> operator>=(const Affine &affine, const double &zero);
std::vector<PositiveConstraint> operator>=(const Affine &lhs, const Affine &rhs);

// represents a constraint like
//      norm2([p_1*x_1 + p_2*x_2 + ... + b_1,   p_3*x_3 + p_4*x_4 + ... + b_2 ])
//        <= p_5*x_5 + p_6*x_6 + ... + b_3
struct SecondOrderConeConstraint
{
    SecondOrderConeConstraint(const Norm2Term &norm2, const AffineSum &affine);
    Norm2Term norm2;
    AffineSum affine;
    friend std::ostream &operator<<(std::ostream &os, const SecondOrderConeConstraint &constraint);
    double evaluate(const std::vector<double> &soln_values) const;
};

std::vector<SecondOrderConeConstraint> operator<=(const Norm2Lhs &norm2lhs, const Affine &affine);
std::vector<SecondOrderConeConstraint> operator>=(const Affine &affine, const Norm2Lhs &norm2lhs);

} // namespace op