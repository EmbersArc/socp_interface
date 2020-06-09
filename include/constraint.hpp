#pragma once

#include "expressions.hpp"

namespace cvx
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

    template <typename Derived>
    std::vector<Constraint> equalTo(const Eigen::MatrixBase<Derived> &lhs, const Scalar &rhs)
    {
        static_assert(std::is_same_v<typename Eigen::MatrixBase<Derived>::Scalar, Scalar>);

        std::vector<Constraint> constraints;

        for (int row = 0; row < lhs.rows(); row++)
        {
            for (int col = 0; col < lhs.cols(); col++)
            {
                constraints.push_back(equalTo(lhs(row, col), rhs));
            }
        }

        return constraints;
    }

    template <typename Derived>
    std::vector<Constraint> equalTo(const Scalar &lhs, const Eigen::MatrixBase<Derived> &rhs)
    {
        return equalTo(rhs, lhs);
    }

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

        if (first_scalar and second_scalar)
        {
            constraints = {equalTo(lhs(0, 0), rhs(0, 0))};
        }
        else if (first_scalar)
        {
            std::vector<Constraint> new_constraints = equalTo(lhs(0, 0), rhs);
            constraints.insert(constraints.end(), new_constraints.begin(), new_constraints.end());
        }
        else if (second_scalar)
        {
            std::vector<Constraint> new_constraints = equalTo(lhs, rhs(0, 0));
            constraints.insert(constraints.end(), new_constraints.begin(), new_constraints.end());
        }
        else
        {
            for (int row = 0; row < rhs.rows(); row++)
            {
                for (int col = 0; col < rhs.cols(); col++)
                {
                    constraints.push_back(equalTo(lhs(row, col), rhs(row, col)));
                }
            }
        }

        return constraints;
    }

    template <typename Derived>
    std::vector<Constraint> lessThan(const Eigen::MatrixBase<Derived> &lhs, const Scalar &rhs)
    {
        static_assert(std::is_same_v<typename Eigen::MatrixBase<Derived>::Scalar, Scalar>);

        std::vector<Constraint> constraints;

        for (int row = 0; row < lhs.rows(); row++)
        {
            for (int col = 0; col < lhs.cols(); col++)
            {
                constraints.push_back(lessThan(lhs(row, col), rhs));
            }
        }

        return constraints;
    }

    template <typename Derived>
    std::vector<Constraint> lessThan(const Scalar &lhs, const Eigen::MatrixBase<Derived> &rhs)
    {
        static_assert(std::is_same_v<typename Eigen::MatrixBase<Derived>::Scalar, Scalar>);

        std::vector<Constraint> constraints;

        for (int row = 0; row < rhs.rows(); row++)
        {
            for (int col = 0; col < rhs.cols(); col++)
            {
                constraints.push_back(lessThan(lhs, rhs(row, col)));
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

        // TODO: check if rhs is linear or first order

        if (first_scalar and second_scalar)
        {
            constraints = {lessThan(lhs(0, 0), rhs(0, 0))};
        }
        else if (first_scalar)
        {
            std::vector<Constraint> new_constraints = lessThan(lhs(0, 0), rhs);
            constraints.insert(constraints.end(), new_constraints.begin(), new_constraints.end());
        }
        else if (second_scalar)
        {
            std::vector<Constraint> new_constraints = lessThan(lhs, rhs(0, 0));
            constraints.insert(constraints.end(), new_constraints.begin(), new_constraints.end());
        }
        else
        {
            for (int row = 0; row < rhs.rows(); row++)
            {
                for (int col = 0; col < rhs.cols(); col++)
                {
                    constraints.push_back(lessThan(lhs(row, col), rhs(row, col)));
                }
            }
        }

        return constraints;
    }

    template <typename DerivedLhs, typename DerivedRhs>
    std::vector<Constraint> greaterThan(const Eigen::MatrixBase<DerivedLhs> &lhs, const Eigen::MatrixBase<DerivedRhs> &rhs)
    {
        return lessThan(rhs, lhs);
    }
    template <typename DerivedLhs, typename DerivedRhs>
    std::vector<Constraint> greaterThan(const Scalar &lhs, const Eigen::MatrixBase<DerivedRhs> &rhs)
    {
        return lessThan(rhs, lhs);
    }
    template <typename DerivedLhs, typename DerivedRhs>
    std::vector<Constraint> greaterThan(const Eigen::MatrixBase<DerivedLhs> &lhs, const Scalar &rhs)
    {
        return lessThan(rhs, lhs);
    }

} // namespace cvx