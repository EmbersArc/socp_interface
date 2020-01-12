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
    (void)zero;

    return EqualityConstraint(affine);
}

std::vector<EqualityConstraint> operator==(const Affine &affine, const double &zero)
{
    assert(zero == 0.0);
    (void)zero;

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
std::vector<EqualityConstraint> operator==(const Affine &lhs, const Affine &rhs)
{
    assert(lhs.shape() == rhs.shape() or lhs.is_scalar() or rhs.is_scalar());

    std::vector<EqualityConstraint> constraints;
    constraints.reserve(std::max(lhs.size(), rhs.size()));
    for (size_t row = 0; row < std::max(lhs.rows(), rhs.rows()); row++)
    {
        for (size_t col = 0; col < std::max(lhs.cols(), rhs.cols()); col++)
        {
            if (not lhs.is_scalar() and not rhs.is_scalar())
            {
                constraints.push_back(rhs.coeff(row, col) + -lhs.coeff(row, col) == 0.);
            }
            else if (lhs.is_scalar())
            {
                constraints.push_back(rhs.coeff(row, col) + -lhs.coeff(0) == 0.);
            }
            else if (rhs.is_scalar())
            {
                constraints.push_back(lhs.coeff(row, col) + -rhs.coeff(0) == 0.);
            }
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
    (void)zero;

    return PositiveConstraint(affine);
}

PositiveConstraint operator<=(const double &zero, const AffineSum &affine)
{
    return affine >= zero;
}

std::vector<PositiveConstraint> operator>=(const Affine &affine, const double &zero)
{
    assert(zero == 0.0);
    (void)zero;

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

std::vector<PositiveConstraint> operator<=(const double &zero, const Affine &affine)
{
    return affine >= zero;
}

std::vector<PositiveConstraint> operator>=(const Affine &lhs, const Affine &rhs)
{
    assert(lhs.shape() == rhs.shape() or lhs.is_scalar() or rhs.is_scalar());

    std::vector<PositiveConstraint> constraints;
    constraints.reserve(std::max(lhs.size(), rhs.size()));
    for (size_t row = 0; row < std::max(lhs.rows(), rhs.rows()); row++)
    {
        for (size_t col = 0; col < std::max(lhs.cols(), rhs.cols()); col++)
        {
            if (not lhs.is_scalar() and not rhs.is_scalar())
            {
                constraints.push_back(-rhs.coeff(row, col) + lhs.coeff(row, col) >= 0.);
            }
            else if (lhs.is_scalar())
            {
                constraints.push_back(lhs.coeff(0) + -rhs.coeff(row, col) >= 0.);
            }
            else if (rhs.is_scalar())
            {
                constraints.push_back(lhs.coeff(row, col) + -rhs.coeff(0) >= 0.);
            }
        }
    }
    return constraints;
}

SecondOrderConeConstraint::SecondOrderConeConstraint(const Norm2Term &norm2, const AffineSum &affine)
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

std::vector<SecondOrderConeConstraint> operator<=(const Norm2Lhs &norm2lhs, const Affine &affine)
{
    assert(norm2lhs.shape() == affine.shape() or affine.is_scalar()); //or norm2lhs.is_scalar()); // Maybe TODO
    std::vector<SecondOrderConeConstraint> constraints;

    for (size_t row = 0; row < norm2lhs.rows(); row++)
    {
        for (size_t col = 0; col < norm2lhs.cols(); col++)
        {
            if (affine.is_scalar())
            {
                constraints.emplace_back(norm2lhs.norm2.coeff(row, col),
                                         affine.coeff(0) + -norm2lhs.affine.coeff(0));
            }
            else
            {
                constraints.emplace_back(norm2lhs.norm2.coeff(row, col),
                                         affine.coeff(row, col) + -norm2lhs.affine.coeff(row, col));
            }
        }
    }
    return constraints;
}

std::vector<SecondOrderConeConstraint> operator>=(const Affine &affine, const Norm2Lhs &norm2lhs)
{
    return norm2lhs <= affine;
}

} // namespace op