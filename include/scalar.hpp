#include <Eigen/Core>

#include "parameter.hpp"
#include "variable.hpp"

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

} // namespace Eigen

namespace op
{

    struct Affine;
    struct Constraint;
    class Scalar;

    struct Term
    {
        Term();

        Parameter parameter;
        std::optional<Variable> variable;

        bool operator==(const Term &other) const;

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
        Affine operator+(const Affine &other) const;
        Affine operator-(const Affine &other) const;
    };

    using MatrixXe = Eigen::Matrix<op::Scalar, Eigen::Dynamic, Eigen::Dynamic>;
    using VectorXe = Eigen::Matrix<op::Scalar, Eigen::Dynamic, 1>;

    class Scalar
    {
    public:
        Scalar() = default;
        explicit Scalar(double x);

        Scalar operator+(const Scalar &other) const;
        Scalar operator-(const Scalar &other) const;
        Scalar operator*(const Scalar &other) const;

        bool operator==(const Scalar &other) const;

        bool isFirstOrder() const;
        bool isSecondOrder() const;
        bool isNorm() const;

    private:
        Affine affine;
        std::vector<Affine> squared_affine;
        bool is_norm = false;

        friend std::ostream &operator<<(std::ostream &os, const Scalar &scalar);

        /**
         * Useful to call .norm() on a matrix.
         * 
         * Possible when only squared expressions are present.
         * 
         */
        friend Scalar sqrt(Scalar &s);
        friend Parameter::operator Scalar() const;
        friend Variable::operator Scalar() const;

        friend std::vector<Constraint> operator<=(const MatrixXe &lhs, const MatrixXe &rhs);
        friend std::vector<Constraint> operator>=(const MatrixXe &lhs, const MatrixXe &rhs);
        friend std::vector<Constraint> operator==(const MatrixXe &lhs, const MatrixXe &rhs);
    };

    MatrixXe createVariables(const std::string &name, size_t rows = 1, size_t cols = 1);

    MatrixXe createParameter(double p);

    MatrixXe createParameter(double *p);

    template <typename Derived>
    MatrixXe createParameter(const Eigen::MatrixBase<Derived> &m)
    {
        MatrixXe parameters(m.rows(), m.cols());

        for (size_t row = 0; row < m.rows(); row++)
        {
            for (size_t col = 0; col < m.cols(); col++)
            {
                parameters(row, col) = Parameter(m.coeffRef(row, col));
            }
        }
        return parameters;
    }

    template <typename Derived>
    MatrixXe createParameter(Eigen::MatrixBase<Derived> *m)
    {
        MatrixXe parameters(m.rows(), m.cols());

        for (size_t row = 0; row < m.rows(); row++)
        {
            for (size_t col = 0; col < m.cols(); col++)
            {
                parameters(row, col) = Parameter(&m.coeffRef(row, col));
            }
        }
        return parameters;
    }

} // namespace op
