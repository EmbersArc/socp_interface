#include "constraint.hpp"

namespace op
{

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

        // If they are both matrices then the dimensions have to match
        if ((lhs.rows() > 1 or lhs.cols() > 1) and
            (rhs.rows() > 1 or rhs.cols() > 1) and
            (lhs.rows() != rhs.rows() or lhs.cols() != rhs.cols()))
        {
            throw std::runtime_error("Invalid dimensions in constraint.");
        }

        const size_t rows = std::max(lhs.rows(), rhs.rows());
        const size_t cols = std::max(lhs.cols(), rhs.cols());

        for (size_t row = 0; row < rows; row++)
        {
            for (size_t col = 0; col < cols; col++)
            {
                Constraint constraint;
                if (lhs(row, col).isFirstOrder())
                {
                    constraint.asPositive(lhs(row, col).affine - rhs(row, col).affine);
                }
                else if (lhs(row, col).isNorm())
                {
                    constraint.asSecondOrderCone(lhs(row, col).squared_affine,
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
