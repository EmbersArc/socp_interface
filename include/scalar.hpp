#include <Eigen/Core>

#include "parameter.hpp"
#include "variable.hpp"

namespace op
{

    // Any term p * x_1 * x_2 * ... * x_n
    // with n in {0, 2} (higher order not supported)
    struct Expression
    {
        Expression();

        Parameter parameter;
        std::vector<Variable> variables;

        // TODO: hmmm
        bool operator==(const Expression &other) const;

        bool is_constant() const;
        bool is_linear() const;
        bool is_quadratic() const;
        size_t get_order() const;

        friend std::ostream &operator<<(std::ostream &os, const Expression &term);
        double evaluate(const std::vector<double> &soln_values) const;
    };
    Expression operator*(const Parameter &parameter, const Variable &variable);

    struct Scalar
    {
        Scalar() = default;
        explicit Scalar(double x);

        std::vector<Expression> polyPart;
        std::vector<Expression> normPart;
        bool squared = false;

        bool operator==(const Scalar &other) const;

        friend std::ostream &operator<<(std::ostream &os, const Scalar &scalar);

        bool norm_valid() const;

        Scalar operator+(const Scalar &other) const;
        Scalar operator-(const Scalar &other) const;
        Scalar operator*(const Scalar &other) const;
        Scalar operator/(const Scalar &other) const;
    };

    /**
     * All variables in polyPart must be squared!
     * 
     * Useful to call .norm() on a matrix
     * 
     */
    Scalar sqrt(Scalar s);

    inline const Scalar &conj(const Scalar &x) { return x; }
    inline const Scalar &real(const Scalar &x) { return x; }

    using MatrixXe = Eigen::Matrix<op::Scalar, Eigen::Dynamic, Eigen::Dynamic>;
    using VectorXe = Eigen::Matrix<op::Scalar, Eigen::Dynamic, 1>;

    MatrixXe createVariables(const std::string &name, size_t rows = 1, size_t cols = 1);

} // namespace op

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
            AddCost = 50,
            MulCost = 200
        };
    };

} // namespace Eigen
