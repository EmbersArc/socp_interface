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

    VectorX createVariables(const std::string &name,
                            size_t rows = 1);

    MatrixX createVariables(const std::string &name,
                            size_t rows,
                            size_t cols);

    Scalar createParameter(double m);

    Scalar createDynamicParameter(double m);

    template <typename Derived>
    inline auto createParameter(const Eigen::MatrixBase<Derived> &m)
    {
        return m.template cast<Scalar>().eval();
    }

    op::Scalar createScalar(double &d)
    {
        return op::Scalar(&d);
    }

    template <typename Derived>
    inline auto createDynamicParameter(Eigen::MatrixBase<Derived> &m)
    {
        return m.unaryExpr(&createScalar);
    }

    template <typename Derived>
    inline auto evaluate(const Eigen::MatrixBase<Derived> &m)
    {
        return m.template cast<double>();
    }

    template <typename Derived>
    inline auto evaluate(const Eigen::SparseMatrixBase<Derived> &m)
    {
        return m.template cast<double>();
    }

    inline const Scalar &conj(const Scalar &x) { return x; }
    inline const Scalar &real(const Scalar &x) { return x; }
    inline Scalar imag(const Scalar &) { return Scalar(0.); }

} // namespace op
