#include <Eigen/Core>

#include "parameter.hpp"
#include "variable.hpp"

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

} // namespace Eigen

namespace op
{

    struct Affine;
    struct Constraint;
    class Expression;

    struct Term
    {
        Term();

        Parameter parameter;
        std::optional<Variable> variable;

        bool operator==(const Term &other) const;
        Term &operator*=(const Parameter &param);

        operator Affine() const;

        friend std::ostream &operator<<(std::ostream &os, const Term &term);
        double evaluate(const std::vector<double> &soln_values) const;
    };
    Term operator*(const Parameter &parameter, const Variable &variable);

    struct Affine
    {
        bool operator==(const Affine &other) const;

        std::vector<Term> terms;
        friend std::ostream &operator<<(std::ostream &os, const Affine &affine);
        double evaluate(const std::vector<double> &soln_values) const;
        Affine &operator+=(const Affine &other);
        Affine operator+(const Affine &other) const;
        Affine operator-(const Affine &other) const;
        Affine operator*(const Affine &other) const;

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
        Expression operator+(const Expression &other) const;
        Expression operator-(const Expression &other) const;
        Expression operator*(const Expression &other) const;

        bool operator==(const Expression &other) const;

        bool isConstant() const;
        bool isFirstOrder() const;
        bool isSecondOrder() const;
        bool isNorm() const;

    private:
        Affine affine;
        std::vector<Affine> squared_affine;
        bool sqrt = false;

        friend std::ostream &operator<<(std::ostream &os, const Expression &scalar);

        /**
         * Useful to call .norm() on a matrix.
         * 
         * Possible when only squared expressions are present.
         * 
         */
        friend Expression sqrt(Expression s);

        friend Parameter::operator Expression() const;
        friend Variable::operator Expression() const;

        friend std::vector<Constraint> operator<=(const MatrixXe &lhs, const MatrixXe &rhs);
        friend std::vector<Constraint> operator>=(const MatrixXe &lhs, const MatrixXe &rhs);
        friend std::vector<Constraint> operator==(const MatrixXe &lhs, const MatrixXe &rhs);
    };

    MatrixXe createVariables(const std::string &name,
                             size_t rows = 1,
                             size_t cols = 1);

    template <typename Derived>
    MatrixXe parameterMatrix(const Eigen::MatrixBase<Derived> &m)
    {
        MatrixXe parameters(m.rows(), m.cols());

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
    MatrixXe parameterMatrix(Eigen::MatrixBase<Derived> *m)
    {
        MatrixXe parameters(m.rows(), m.cols());

        for (int row = 0; row < m.rows(); row++)
        {
            for (int col = 0; col < m.cols(); col++)
            {
                parameters(row, col) = Parameter(&m.coeffRef(row, col));
            }
        }
        return parameters;
    }

} // namespace op
