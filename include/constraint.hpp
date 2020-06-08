#pragma once

#include "expression.hpp"

namespace op
{

    struct EqualityConstraint
    {
        Affine affine;
        friend std::ostream &operator<<(std::ostream &os, const EqualityConstraint &constraint);
    };

    struct PositiveConstraint
    {
        Affine affine;
        friend std::ostream &operator<<(std::ostream &os, const PositiveConstraint &constraint);
    };

    struct SecondOrderConeConstraint
    {
        std::vector<Affine> norm;
        Affine affine;
        friend std::ostream &operator<<(std::ostream &os, const SecondOrderConeConstraint &constraint);
    };

    class Constraint
    {
    public:
        enum Type
        {
            Equality,
            Positive,
            SecondOrderCone
        };

        Type getType() const;
        void asEquality(const Affine &affine);
        void asPositive(const Affine &affine);
        void asSecondOrderCone(const std::vector<Affine> &norm, const Affine &affine);

        friend std::ostream &operator<<(std::ostream &os, const Constraint &constraint);

        using constraint_variant_t = std::variant<EqualityConstraint,
                                                  PositiveConstraint,
                                                  SecondOrderConeConstraint>;
        constraint_variant_t data;
    };

    Constraint equalTo(const Scalar &lhs, const Scalar &rhs);
    Constraint lessThan(const Scalar &lhs, const Scalar &rhs);
    Constraint greaterThan(const Scalar &lhs, const Scalar &rhs);

    template <typename DerivedLhs, typename DerivedRhs>
    std::vector<Constraint> equalTo(const Eigen::MatrixBase<DerivedLhs> &lhs, const Eigen::MatrixBase<DerivedRhs> &rhs)
    {
        static_assert(std::is_same_v<typename Eigen::MatrixBase<DerivedLhs>::Scalar, Scalar>);
        static_assert(std::is_same_v<typename Eigen::MatrixBase<DerivedRhs>::Scalar, Scalar>);

        std::vector<Constraint> constraints;

        const bool first_scalar = lhs.rows() == 1 and lhs.cols() == 1;
        const bool second_scalar = rhs.rows() == 1 and rhs.cols() == 1;
        const bool same_shape = lhs.rows() == rhs.rows() and lhs.cols() == rhs.cols();

        // If they are both matrices then the dimensions have to match
        if (not first_scalar and not second_scalar and not same_shape)
        {
            throw std::runtime_error("Invalid dimensions in constraint.");
        }

        const size_t rows = std::max(lhs.rows(), rhs.rows());
        const size_t cols = std::max(lhs.cols(), rhs.cols());

        for (size_t row = 0; row < rows; row++)
        {
            for (size_t col = 0; col < cols; col++)
            {
                size_t l_row = 0;
                size_t l_col = 0;
                size_t r_row = 0;
                size_t r_col = 0;

                if (not first_scalar)
                {
                    l_row = row;
                    l_col = col;
                }
                if (not second_scalar)
                {
                    r_row = row;
                    r_col = col;
                }

                Constraint constraint = equalTo(lhs(l_row, l_col), rhs(r_row, r_col));
                constraints.push_back(constraint);
            }
        }

        return constraints;
    }

    template <typename DerivedLhs, typename DerivedRhs>
    std::vector<Constraint> lessThan(const Eigen::MatrixBase<DerivedLhs> &lhs, const Eigen::MatrixBase<DerivedRhs> &rhs)
    {
        static_assert(std::is_same_v<typename Eigen::MatrixBase<DerivedLhs>::Scalar, Scalar>);
        static_assert(std::is_same_v<typename Eigen::MatrixBase<DerivedRhs>::Scalar, Scalar>);

        std::vector<Constraint> constraints;

        const bool first_scalar = lhs.rows() == 1 and lhs.cols() == 1;
        const bool second_scalar = rhs.rows() == 1 and rhs.cols() == 1;
        const bool same_shape = lhs.rows() == rhs.rows() and lhs.cols() == rhs.cols();

        // If they are both matrices then the dimensions have to match
        if (not first_scalar and not second_scalar and not same_shape)
        {
            throw std::runtime_error("Invalid dimensions in constraint.");
        }

        const size_t rows = std::max(lhs.rows(), rhs.rows());
        const size_t cols = std::max(lhs.cols(), rhs.cols());

        // TODO: check if rhs is linear or first order

        for (size_t row = 0; row < rows; row++)
        {
            for (size_t col = 0; col < cols; col++)
            {
                size_t l_row = 0;
                size_t l_col = 0;
                size_t r_row = 0;
                size_t r_col = 0;

                if (not first_scalar)
                {
                    l_row = row;
                    l_col = col;
                }
                if (not second_scalar)
                {
                    r_row = row;
                    r_col = col;
                }

                Constraint constraint = lessThan(lhs(l_row, l_col), rhs(r_row, r_col));

                constraints.push_back(constraint);
            }
        }

        return constraints;
    }

    template <typename DerivedLhs, typename DerivedRhs>
    std::vector<Constraint> greaterThan(const Eigen::MatrixBase<DerivedLhs> &lhs, const Eigen::MatrixBase<DerivedRhs> &rhs)
    {
        return lessThan(rhs, lhs);
    }

} // namespace op