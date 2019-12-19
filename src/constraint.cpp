#include "constraint.hpp"

#include <sstream>
#include <cassert>

namespace op
{

std::ostream &operator<<(std::ostream &os, const PostiveConstraint &constraint)
{
    os << constraint.lhs << " >= 0";
    return os;
}

std::ostream &operator<<(std::ostream &os, const EqualityConstraint &constraint)
{
    os << constraint.lhs << " == 0";
    return os;
}

std::ostream &operator<<(std::ostream &os, const SecondOrderConeConstraint &constraint)
{
    os << constraint.lhs << " <= " << constraint.rhs;
    return os;
}

Norm2 norm2(const std::vector<AffineExpression> &affineExpressions)
{
    Norm2 n;
    n.arguments = affineExpressions;
    return n;
}

SecondOrderConeConstraint operator<=(const Norm2 &lhs, const AffineExpression &rhs)
{
    SecondOrderConeConstraint socc;
    socc.lhs = lhs;
    socc.rhs = rhs;
    return socc;
}

SecondOrderConeConstraint operator>=(const AffineExpression &lhs, const Norm2 &rhs)
{
    return rhs <= lhs;
}

SecondOrderConeConstraint operator<=(const Norm2 &lhs, const double &rhs)
{
    SecondOrderConeConstraint socc;
    socc.lhs = lhs;
    socc.rhs = Parameter(rhs);
    return socc;
}

SecondOrderConeConstraint operator>=(const double &lhs, const Norm2 &rhs)
{
    return rhs <= lhs;
}

PostiveConstraint operator>=(const AffineExpression &lhs, const double &zero)
{
    assert(zero == 0.0);
    PostiveConstraint result;
    result.lhs = lhs;
    return result;
}

PostiveConstraint operator<=(const double &zero, const AffineExpression &rhs)
{
    return rhs >= zero;
}

EqualityConstraint operator==(const AffineExpression &lhs, const double &zero)
{
    assert(zero == 0.0);
    EqualityConstraint result;
    result.lhs = lhs;
    return result;
}

double SecondOrderConeConstraint::evaluate(const std::vector<double> &soln_values) const
{
    return (lhs.evaluate(soln_values) - rhs.evaluate(soln_values));
}

double PostiveConstraint::evaluate(const std::vector<double> &soln_values) const
{
    return -lhs.evaluate(soln_values);
}

double EqualityConstraint::evaluate(const std::vector<double> &soln_values) const
{
    return -lhs.evaluate(soln_values);
}

} // namespace op