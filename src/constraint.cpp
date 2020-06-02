#include "constraint.hpp"

namespace op
{

    std::ostream &operator<<(std::ostream &os, const EqualityConstraint &constraint)
    {
        os << constraint.affine << " == 0";
        return os;
    }
    std::ostream &operator<<(std::ostream &os, const PositiveConstraint &constraint)
    {
        os << constraint.affine << " >= 0";
        return os;
    }
    std::ostream &operator<<(std::ostream &os, const SecondOrderConeConstraint &constraint)
    {
        os << "(";
        for (size_t i = 0; i < constraint.norm.size(); i++)
        {
            os << "(" << constraint.norm[i] << ")^2 ";
            if (i != constraint.norm.size() - 1)
            {
                os << " + ";
            }
        }
        os << ")^(1/2)";

        os << " <= " << constraint.affine;

        return os;
    }

    Constraint::Type Constraint::getType() const
    {
        return Type(data.index());
    }

    void Constraint::asEquality(const Affine &affine)
    {
        EqualityConstraint constraint;
        constraint.affine = affine;
        data = constraint;
    }

    void Constraint::asPositive(const Affine &affine)
    {

        PositiveConstraint constraint;
        constraint.affine = affine;
        data = constraint;
    }

    void Constraint::asSecondOrderCone(const std::vector<Affine> &norm, const Affine &affine)
    {
        SecondOrderConeConstraint constraint;
        constraint.norm = norm;
        constraint.affine = affine;
        data = constraint;
    }

    std::vector<Constraint> operator==(const MatrixXe &lhs, const MatrixXe &rhs)
    {
        std::vector<Constraint> constraints;

        // If they are both matrices then the dimensions have to match
        if ((lhs.rows() > 1 or lhs.cols() > 1) and
            (rhs.rows() > 1 or rhs.cols() > 1) and
            (lhs.rows() != rhs.rows() or lhs.cols() != rhs.cols()))
        {
            throw std::runtime_error("Invalid dimensions in constraint!");
        }

        const size_t rows = std::max(lhs.rows(), rhs.rows());
        const size_t cols = std::max(lhs.cols(), rhs.cols());

        for (size_t row = 0; row < rows; row++)
        {
            for (size_t col = 0; col < cols; col++)
            {
                Constraint constraint;
                constraint.asEquality(lhs(row, col).affine - rhs(row, col).affine);
                constraints.push_back(constraint);
            }
        }

        return constraints;
    }

    std::vector<Constraint> operator<=(const MatrixXe &lhs, const MatrixXe &rhs)
    {
        std::vector<Constraint> constraints;

        const bool first_scalar = lhs.rows() == 1 and lhs.cols() == 1;
        const bool second_scalar = rhs.rows() == 1 and rhs.cols() == 1;
        const bool same_shape = lhs.rows() == rhs.rows() and lhs.cols() == rhs.cols();

        // If they are both matrices then the dimensions have to match
        if (not (first_scalar or second_scalar) and not same_shape)
        {
            throw std::runtime_error("Invalid dimensions in constraint.");
        }

        const size_t rows = std::max(lhs.rows(), rhs.rows());
        const size_t cols = std::max(lhs.cols(), rhs.cols());

        // TODO: check if rhs is linear of first order

        for (size_t row = 0; row < rows; row++)
        {
            for (size_t col = 0; col < cols; col++)
            {
                Constraint constraint;
                if (lhs(row, col).getOrder() < 2)
                {
                    if (lhs(row, col).getOrder() > 0 or rhs(row, col).getOrder() > 0)
                        constraint.asPositive(lhs(row, col).affine - rhs(row, col).affine);
                }
                else if (lhs(row, col).isNorm())
                {
                    std::vector<Affine> norm2_terms;
                    for (const std::vector<Affine> &affine : lhs(row, col).higher_order)
                    {
                        norm2_terms.push_back(affine[0]);
                    }
                    constraint.asSecondOrderCone(norm2_terms,
                                                 rhs(row, col).affine - lhs(row, col).affine);
                }
                else
                {
                    throw std::runtime_error("Found unsupported term in constraint.");
                }
                constraints.push_back(constraint);
            }
        }

        return constraints;
    }

    std::vector<Constraint> operator>=(const MatrixXe &lhs, const MatrixXe &rhs)
    {
        return rhs <= lhs;
    }

} // namespace op
