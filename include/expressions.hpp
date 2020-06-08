#pragma once

#include "parameter.hpp"
#include "variable.hpp"

#include <Eigen/Core>

namespace Eigen
{

    template <>
    struct NumTraits<op::Scalar>
        : NumTraits<double>
    {
        using Real = op::Scalar;
        using NonInteger = op::Scalar;
        using Nested = op::Scalar;

        enum
        {
            IsComplex = 0,
            IsInteger = 0,
            IsSigned = 1,
            RequireInitialization = 1,
            ReadCost = 10,
            AddCost = 200,
            MulCost = 200,
        };
    };

    namespace internal
    {
        // Needed for .diagonal() and possibly other Eigen access functions.
        bool operator==(const op::Scalar &lhs, const op::Scalar &rhs);
    } // namespace internal

} // namespace Eigen

namespace op
{

    class Affine;
    class Constraint;
    class Scalar;
    class OptimizationProblem;
    class WrapperBase;

    class Term
    {
    public:
        Term();

        Parameter parameter;
        Variable variable;

        bool operator==(const Term &other) const;
        Term &operator*=(const Parameter &param);

        operator Affine() const;

        friend std::ostream &operator<<(std::ostream &os, const Term &term);
        double evaluate() const;
    };
    Term operator*(const Parameter &parameter, const Variable &variable);

    class Affine
    {
    public:
        bool operator==(const Affine &other) const;

        Parameter constant = Parameter(0.);
        std::vector<Term> terms;

        friend std::ostream &operator<<(std::ostream &os, const Affine &affine);
        double evaluate() const;
        Affine &operator+=(const Affine &other);
        Affine operator+(const Affine &other) const;
        Affine operator-(const Affine &other) const;
        Affine operator*(const Affine &other) const;

        bool isZero() const;
        bool isConstant() const;
        bool isFirstOrder() const;
    };

    using MatrixX = Eigen::Matrix<op::Scalar, Eigen::Dynamic, Eigen::Dynamic>;
    using VectorX = Eigen::Matrix<op::Scalar, Eigen::Dynamic, 1>;

    class Scalar
    {
    public:
        Scalar() = default;
        explicit Scalar(int x);
        explicit Scalar(double x);
        explicit Scalar(double *x);

        Scalar &operator+=(const Scalar &other);
        Scalar &operator-=(const Scalar &other);
        Scalar operator+(const Scalar &other) const;
        Scalar operator-(const Scalar &other) const;
        Scalar operator*(const Scalar &other) const;

        friend bool Eigen::internal::operator==(const op::Scalar &lhs, const op::Scalar &rhs);

        double evaluate() const;
        size_t getOrder() const;
        bool isNorm() const;

        friend OptimizationProblem;
        friend WrapperBase;

        operator double() const;

    private:
        Affine affine;
        std::vector<std::vector<Affine>> higher_order;
        bool sqrt = false;

        friend std::ostream &operator<<(std::ostream &os, const Scalar &scalar);

        /**
         * Useful to call .norm() on a matrix.
         * 
         * Possible when only squared expressions are present.
         * 
         */
        friend Scalar sqrt(const Scalar &scalar);

        friend Parameter::operator Scalar() const;
        friend Variable::operator Scalar() const;

        friend Constraint equalTo(const Scalar &lhs, const Scalar &rhs);
        friend Constraint lessThan(const Scalar &lhs, const Scalar &rhs);
    };

    VectorX var(const std::string &name,
                size_t rows = 1);

    MatrixX var(const std::string &name,
                size_t rows,
                size_t cols);

    Scalar par(double m);

    Scalar dynpar(double m);

    template <typename Derived>
    inline auto par(const Eigen::MatrixBase<Derived> &m)
    {
        return m.template cast<Scalar>().eval();
    }

    template <typename Derived>
    auto dynpar(Eigen::MatrixBase<Derived> &m)
    {
        // TODO: Maybe find something better here

        auto result = m.template cast<Scalar>().eval();

        for (int row = 0; row < m.rows(); row++)
        {
            for (int col = 0; col < m.cols(); col++)
            {
                result.coeffRef(row, col) = Scalar(&m.coeffRef(row, col));
            }
        }

        return result;
    }

    template <typename Derived>
    inline auto eval(const Eigen::MatrixBase<Derived> &m)
    {
        return m.template cast<double>();
    }

    template <typename Derived>
    inline auto eval(const Eigen::SparseMatrixBase<Derived> &m)
    {
        return m.template cast<double>();
    }

    inline const Scalar &conj(const Scalar &x) { return x; }
    inline const Scalar &real(const Scalar &x) { return x; }
    inline Scalar imag(const Scalar &) { return Scalar(0.); }

} // namespace op
