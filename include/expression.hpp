#pragma once

#include "parameter.hpp"
#include "variable.hpp"

#include <optional>

namespace op
{

namespace internal
{

// A linear term (p_1*x_1) or constant term (p_1)
struct AffineTerm
{
    AffineTerm();
    explicit AffineTerm(const ParameterSource &parameter);
    explicit AffineTerm(const VariableSource &variable);
    AffineTerm(const ParameterSource &parameter,
               const VariableSource &variable);

    operator internal::AffineSum() const;

    ParameterSource parameter;
    std::optional<VariableSource> variable; // a missing Variable represents a constant 1.0
    friend std::ostream &operator<<(std::ostream &os, const AffineTerm &term);
    double evaluate(const std::vector<double> &soln_values) const;
    AffineTerm operator*(const ParameterSource &parameter) const;
    AffineTerm &operator*=(const ParameterSource &parameter);
};
AffineTerm operator*(const ParameterSource &parameter, const VariableSource &variable);
AffineTerm operator*(const double &const_parameter, const VariableSource &variable);

// A term like (p_1*x_1 + p_2*x_2 + ... + b)
struct AffineSum
{
    AffineSum() = default;
    explicit AffineSum(const ParameterSource &parameter);
    explicit AffineSum(const VariableSource &variable);
    explicit AffineSum(const AffineTerm &term);

    std::vector<AffineTerm> terms;
    friend std::ostream &operator<<(std::ostream &os, const AffineSum &expression);
    double evaluate(const std::vector<double> &soln_values) const;
    AffineSum &operator+=(const AffineSum &other);
    AffineSum operator+(const AffineSum &other) const;
    AffineSum operator*(const ParameterSource &parameter) const;
    AffineSum operator-() const;
    size_t clean();
    bool is_constant() const;
};
AffineSum operator+(const double &lhs, const AffineSum &rhs);
AffineSum operator+(const AffineSum &lhs, const double &rhs);

} // namespace internal

// A scalar/vector/matrix of affine expressions
class Affine : public DynamicMatrix<internal::AffineSum, Affine>
{
public:
    using DynamicMatrix<internal::AffineSum, Affine>::DynamicMatrix;
    explicit Affine(const Parameter &parameter);
    explicit Affine(const Variable &variable);
    explicit Affine(const internal::AffineSum &expression);
    friend std::ostream &operator<<(std::ostream &os, const Affine &expression);
    Affine &operator+=(const Affine &other);
    Affine operator-() const;
};
Affine operator+(const Affine &lhs, const Affine &rhs);
Affine operator*(const Parameter &parameter, const Variable &variable);
Affine operator*(const Variable &variable, const Parameter &parameter);
Affine operator*(const Parameter &parameter, const Affine &affine);

namespace internal
{

// A term like norm2([p_1*x_1 + p_2*x_2 + ... + b_1,   p_3*x_3 + p_4*x_4 + ... + b_2, ... ])
struct Norm2Term
{
    Norm2Term() = default;
    explicit Norm2Term(const Affine &affine);
    std::vector<internal::AffineSum> arguments;
    friend std::ostream &operator<<(std::ostream &os, const Norm2Term &norm2);
    double evaluate(const std::vector<double> &soln_values) const;
};

} // namespace internal

struct SOCLhs;

class Norm2 : public DynamicMatrix<internal::Norm2Term, Norm2>
{
public:
    using DynamicMatrix<internal::Norm2Term, Norm2>::DynamicMatrix;
    explicit Norm2(const Affine &affine);
    Norm2(const Affine &affine, size_t axis);
    operator SOCLhs() const;
};

Affine operator-(const Variable &variable);

Affine sum(const Affine &affine);
Affine sum(const Affine &affine, size_t axis);

struct SOCLhs
{
    Norm2 norm2;
    std::optional<Affine> affine;
    bool is_scalar() const;
    size_t rows() const;
    size_t cols() const;
    std::pair<size_t, size_t> shape() const;
    SOCLhs operator+(const Affine &affine) const;
    SOCLhs &operator+=(const Affine &affine);
};

SOCLhs operator+(const Norm2 &norm2, const Affine &affine);

} // namespace op