#include "parameter.hpp"
#include "variable.hpp"

#include <Eigen/Core>

namespace Eigen
{

    template <>
    struct NumTraits<op::Expression>
        : NumTraits<double>
    {
        using Real = op::Expression;
        using NonInteger = op::Expression;
        using Nested = op::Expression;

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
        bool operator==(const op::Expression &lhs, const op::Expression &rhs);
    } // namespace internal

} // namespace Eigen

namespace op
{

    class Affine;
    class Constraint;
    class Expression;
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

    using MatrixXe = Eigen::Matrix<op::Expression, Eigen::Dynamic, Eigen::Dynamic>;
    using VectorXe = Eigen::Matrix<op::Expression, Eigen::Dynamic, 1>;

    class Expression
    {
    public:
        Expression() = default;
        explicit Expression(double x);

        Expression &operator+=(const Expression &other);
        Expression &operator-=(const Expression &other);
        Expression operator+(Expression other) const;
        Expression operator-(const Expression &other) const;
        Expression operator*(const Expression &other) const;

        friend bool Eigen::internal::operator==(const op::Expression &lhs, const op::Expression &rhs);

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

        friend std::ostream &operator<<(std::ostream &os, const Expression &scalar);

        /**
         * Useful to call .norm() on a matrix.
         * 
         * Possible when only squared expressions are present.
         * 
         */
        friend Expression sqrt(const Expression &expression);

        friend Parameter::operator Expression() const;
        friend Variable::operator Expression() const;

        friend Constraint equalTo(const Expression &lhs, const Expression &rhs);
        friend Constraint lessThan(const Expression &lhs, const Expression &rhs);
    };

    VectorXe createVariables(const std::string &name,
                             size_t rows = 1);

    MatrixXe createVariables(const std::string &name,
                             size_t rows,
                             size_t cols);

    Expression createParameter(double m);

    Expression createParameter(double *m);

    template <typename Derived>
    auto createParameter(const Eigen::MatrixBase<Derived> &m)
    {
        Eigen::Matrix<Expression, Eigen::MatrixBase<Derived>::RowsAtCompileTime, Eigen::MatrixBase<Derived>::ColsAtCompileTime> parameters(m.rows(), m.cols());

        for (int row = 0; row < m.rows(); row++)
        {
            for (int col = 0; col < m.cols(); col++)
            {
                parameters(row, col) = Parameter(m.coeff(row, col));
            }
        }
        return parameters;
    }

    template <typename Derived>
    auto createParameter(Eigen::MatrixBase<Derived> *m)
    {
        Eigen::Matrix<Expression, Eigen::MatrixBase<Derived>::RowsAtCompileTime, Eigen::MatrixBase<Derived>::ColsAtCompileTime> parameters(m->rows(), m->cols());

        for (int row = 0; row < m->rows(); row++)
        {
            for (int col = 0; col < m->cols(); col++)
            {
                parameters(row, col) = Parameter(&m->coeffRef(row, col));
            }
        }
        return parameters;
    }

    // TODO: doesn't work
    template <typename Derived>
    inline auto evaluate(Eigen::MatrixBase<Derived> m)
    {
        return m.template cast<double>().eval();
    }

    inline const Expression &conj(const Expression &x) { return x; }
    inline const Expression &real(const Expression &x) { return x; }
    inline Expression imag(const Expression &) { return Expression(0.); }

} // namespace op
