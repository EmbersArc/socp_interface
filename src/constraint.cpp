#include "constraint.hpp"

#include <sstream>
#include <cassert>

namespace op
{

EqualityConstraint::EqualityConstraint(const AffineSum &affine)
    : affine(affine) {}

EqualityConstraint operator==(const AffineSum &affine, const double &zero)
{
    assert(zero == 0.0);
    return EqualityConstraint(affine);
}

std::vector<EqualityConstraint> operator==(const AffineExpression &affine, const double &zero)
{
    assert(zero == 0.0);

    std::vector<EqualityConstraint> constraints;
    constraints.reserve(affine.size());
    for (size_t row = 0; row < affine.rows(); row++)
    {
        for (size_t col = 0; col < affine.cols(); col++)
        {
            constraints.push_back(affine.coeff(row, col) == 0.0);
        }
    }
    return constraints;
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

PositiveConstraint::PositiveConstraint(const AffineSum &affine)
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

PositiveConstraint operator>=(const AffineSum &affine, const double &zero)
{
    assert(zero == 0.0);
    return PositiveConstraint(affine);
}

PositiveConstraint operator<=(const double &zero, const AffineSum &affine)
{
    return affine >= zero;
}

std::vector<PositiveConstraint> operator>=(const AffineExpression &affine, const double &zero)
{
    assert(zero == 0.0);
    std::vector<PositiveConstraint> constraints;
    for (size_t row = 0; row < affine.rows(); row++)
    {
        for (size_t col = 0; col < affine.cols(); col++)
        {
            constraints.push_back(affine.coeff(row, col) >= 0.0);
        }
    }
    return constraints;
}

std::vector<PositiveConstraint> operator<=(const double &zero, const AffineExpression &affine)
{
    return affine >= zero;
}

SecondOrderConeConstraint::SecondOrderConeConstraint(const Norm2 &norm2, const AffineSum &affine)
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
    return SecondOrderConeConstraint(norm2, affine.coeff(0));
}

SecondOrderConeConstraint operator>=(const AffineExpression &affine, const Norm2 &norm2)
{
    return norm2 <= affine;
}

SecondOrderConeConstraint operator<=(const Norm2 &norm2, const double &constant)
{
    return SecondOrderConeConstraint(norm2, ParameterSource(constant));
}

SecondOrderConeConstraint operator>=(const double &constant, const Norm2 &norm2)
{
    return norm2 <= constant;
}

} // namespace op