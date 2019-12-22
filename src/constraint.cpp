#include "constraint.hpp"

#include <sstream>
#include <cassert>

namespace op
{

EqualityConstraint::EqualityConstraint(const AffineExpression &affine)
    : affine(affine) {}

EqualityConstraint operator==(const AffineExpression &affine, const double &zero)
{
    assert(zero == 0.0);
    return EqualityConstraint(affine);
}

std::ostream &operator<<(std::ostream &os, const EqualityConstraint &constraint)
{
    os << constraint.affine << " == 0";
    return os;
}

double EqualityConstraint::evaluate(const std::vector<double> &soln_values) const
{
    return -affine.evaluate(soln_values);
}

PositiveConstraint::PositiveConstraint(const AffineExpression &affine)
    : affine(affine) {}

std::ostream &operator<<(std::ostream &os, const PositiveConstraint &constraint)
{
    os << constraint.affine << " >= 0";
    return os;
}

double PositiveConstraint::evaluate(const std::vector<double> &soln_values) const
{
    return -affine.evaluate(soln_values);
}

PositiveConstraint operator>=(const AffineExpression &affine, const double &zero)
{
    assert(zero == 0.0);
    return PositiveConstraint(affine);
}

PositiveConstraint operator<=(const double &zero, const AffineExpression &affine)
{
    return affine >= zero;
}

SecondOrderConeConstraint::SecondOrderConeConstraint(const Norm2 &norm2, const AffineExpression &affine)
    : norm2(norm2), affine(affine) {}

std::ostream &operator<<(std::ostream &os, const SecondOrderConeConstraint &constraint)
{
    os << constraint.norm2 << " <= " << constraint.affine;
    return os;
}

double SecondOrderConeConstraint::evaluate(const std::vector<double> &soln_values) const
{
    return (norm2.evaluate(soln_values) - affine.evaluate(soln_values));
}

SecondOrderConeConstraint operator<=(const Norm2 &norm2, const AffineExpression &affine)
{
    return SecondOrderConeConstraint(norm2, affine);
}

SecondOrderConeConstraint operator>=(const AffineExpression &lhs, const Norm2 &rhs)
{
    return rhs <= lhs;
}

SecondOrderConeConstraint operator<=(const Norm2 &norm2, const double &constant)
{
    return SecondOrderConeConstraint(norm2, Parameter(constant));
}

SecondOrderConeConstraint operator>=(const double &lhs, const Norm2 &rhs)
{
    return rhs <= lhs;
}

} // namespace op