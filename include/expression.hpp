#pragma once

#include "parameter.hpp"
#include "variable.hpp"

#include <optional>

namespace op
{

// A linear term (p_1*x_1) or constant term (p_1)
struct AffineTerm
{
    AffineTerm();
    explicit AffineTerm(const ParameterSource &parameter);
    explicit AffineTerm(const VariableSource &variable);
    AffineTerm(const ParameterSource &parameter,
               const VariableSource &variable);

    operator AffineSum() const;

    ParameterSource parameter;
    std::optional<VariableSource> variable; // a missing Variable represents a constant 1.0
    friend std::ostream &operator<<(std::ostream &os, const AffineTerm &term);
    double evaluate(const std::vector<double> &soln_values) const;
    AffineTerm operator*(const ParameterSource &parameter) const;
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
};
AffineSum operator+(const double &lhs, const AffineSum &rhs);
AffineSum operator+(const AffineSum &lhs, const double &rhs);

// A scalar/vector/matrix of affine expressions
class Affine : public DynamicMatrix<AffineSum, Affine>
{
public:
    using DynamicMatrix<AffineSum, Affine>::DynamicMatrix;
    explicit Affine(const Parameter &parameter);
    explicit Affine(const Variable &variable);
    explicit Affine(const AffineSum &expression);
    friend std::ostream &operator<<(std::ostream &os, const Affine &expression);
    Affine &operator+=(const Affine &other);
};
Affine operator+(const Affine &lhs, const Affine &rhs);
Affine operator*(const Parameter &parameter, const Variable &variable);
Affine operator*(const Variable &variable, const Parameter &parameter);
Affine operator*(const double &const_parameter, const Variable &variable);
Affine operator*(const Parameter &parameter, const Affine &affine);

// A term like norm2([p_1*x_1 + p_2*x_2 + ... + b_1,   p_3*x_3 + p_4*x_4 + ... + b_2 ])
struct Norm2Term
{
    Norm2Term() = default;
    explicit Norm2Term(const Affine &affine);
    std::vector<AffineSum> arguments;
    friend std::ostream &operator<<(std::ostream &os, const Norm2Term &norm2);
    double evaluate(const std::vector<double> &soln_values) const;
};

class Norm2 : public DynamicMatrix<Norm2Term, Norm2>
{
public:
    using DynamicMatrix<Norm2Term, Norm2>::DynamicMatrix;
    Norm2(const Affine &affine, size_t axis = 0);
};

Affine operator-(const Variable &variable);

Affine sum(const Affine &affine);
Affine sum(const Affine &affine, size_t axis);

} // namespace op