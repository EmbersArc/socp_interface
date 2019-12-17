#include "constraint.hpp"

#include <sstream>
#include <cassert>

namespace op
{


std::string PostiveConstraint::print() const
{
    std::ostringstream s;
    s << lhs.print() << " >= 0";
    return s.str();
}

std::string EqualityConstraint::print() const
{
    std::ostringstream s;
    s << lhs.print() << " == 0";
    return s.str();
}

std::string SecondOrderConeConstraint::print() const
{
    std::ostringstream s;
    s << lhs.print() << " <= " << rhs.print();
    return s.str();
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

EqualityConstraint operator==(const double &zero, const AffineExpression &rhs)
{
    return rhs == zero;
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

}