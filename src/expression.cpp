#include "expression.hpp"

#include <sstream>
#include <cassert>
#include <numeric>
#include <cmath>

namespace op
{

AffineTerm::AffineTerm()
    : parameter(0.) {}
AffineTerm::AffineTerm(const ParameterSource &parameter)
    : parameter(parameter) {}
AffineTerm::AffineTerm(const VariableSource &variable)
    : parameter(1.0), variable(variable) {}
AffineTerm::AffineTerm(const ParameterSource &parameter, const VariableSource &variable)
    : parameter(parameter), variable(variable) {}
std::ostream &operator<<(std::ostream &os, const AffineTerm &term)
{
    os << term.parameter.getValue();
    if (term.variable)
        os << "*" << term.variable.value();
    return os;
}
double AffineTerm::evaluate(const std::vector<double> &soln_values) const
{
    double p = parameter.getValue();
    if (variable)
    {
        return p * soln_values[variable.value().getProblemIndex()];
    }
    else
    {
        return p;
    }
}
AffineTerm operator*(const ParameterSource &parameter, const VariableSource &variable)
{
    return AffineTerm(parameter, variable);
}
AffineTerm operator*(const double &const_parameter, const VariableSource &variable)
{
    return AffineTerm(const_parameter, variable);
}

AffineExpression::AffineExpression(const ParameterSource &parameter) : terms({parameter}) {}
AffineExpression::AffineExpression(const AffineTerm &term) : terms{term} {}
std::ostream &operator<<(std::ostream &os, const AffineExpression &expression)
{
    for (size_t i = 0; i < expression.terms.size(); i++)
    {
        if (i > 0)
            os << " + ";
        os << expression.terms[i];
    }
    return os;
}
double AffineExpression::evaluate(const std::vector<double> &soln_values) const
{
    double sum = 0.;
    for (const auto &term : terms)
    {
        sum += term.evaluate(soln_values);
    }
    return sum;
}

AffineExpression operator+(const AffineExpression &lhs, const AffineExpression &rhs)
{
    AffineExpression result;
    result.terms.insert(result.terms.end(), lhs.terms.begin(), lhs.terms.end());
    result.terms.insert(result.terms.end(), rhs.terms.begin(), rhs.terms.end());
    return result;
}

AffineExpression operator+(const AffineExpression &lhs, const double &rhs)
{
    AffineExpression result;
    result.terms.insert(result.terms.end(), lhs.terms.begin(), lhs.terms.end());
    result.terms.push_back(ParameterSource(rhs));
    return result;
}

AffineExpression operator+(const double &rhs, const AffineExpression &lhs)
{
    return lhs + rhs;
}

Affine::Affine(const Parameter &parameter)
{
    for (size_t row = 0; row < parameter.rows(); row++)
    {
        expressions.push_back({});
        for (size_t col = 0; col < parameter.cols(); col++)
        {
            expressions.back().emplace_back(parameter(row, col));
        }
    }
}

Affine::Affine(const AffineExpression &expression)
    : expressions({{expression}}) {}

size_t Affine::rows() const
{
    return expressions.size();
}

size_t Affine::cols() const
{
    return expressions.front().size();
}

std::pair<size_t, size_t> Affine::shape() const
{
    return {rows(), cols()};
}

AffineExpression Affine::operator()(const size_t row, const size_t col) const
{
    return expressions[row][col];
}

std::ostream &operator<<(std::ostream &os, const Norm2 &norm2)
{
    os << "Norm2([";
    for (size_t i = 0; i < norm2.arguments.size(); i++)
    {
        if (i > 0)
            os << ", ";
        os << norm2.arguments[i];
    }
    os << "])";
    return os;
}

Affine operator+(const Affine &lhs, const Affine &rhs)
{
    assert(lhs.shape() == rhs.shape());
    Affine result;
    result.expressions = lhs.expressions;
    for (size_t row = 0; row < lhs.rows(); row++)
    {
        for (size_t col = 0; col < lhs.cols(); col++)
        {
            result.expressions[row][col] = result(row, col) + rhs(row, col);
        }
    }
    return result;
}

Affine operator*(const Parameter &parameter, const Variable &variable)
{
    bool first_scalar = parameter.rows() == 1 and parameter.cols() == 1;
    bool second_scalar = variable.rows() == 1 and variable.cols() == 1;
    bool both_scalar = first_scalar and second_scalar;
    bool both_matrix = not first_scalar and not second_scalar;

    if (both_scalar)
    {
        return Affine(parameter(0) * variable(0));
    }
    else if (both_matrix)
    {
        assert(parameter.cols() == variable.rows());
        Affine result;
        for (size_t row = 0; row < parameter.rows(); row++)
        {
            std::vector<AffineExpression> expression_row;
            for (size_t col = 0; col < variable.cols(); col++)
            {
                AffineExpression expression;
                for (size_t inner = 0; inner < parameter.cols(); inner++)
                {
                    expression.terms.emplace_back(parameter(row, inner), variable(inner, col));
                }
                expression_row.push_back(expression);
            }
            result.expressions.push_back(expression_row);
        }
        return result;
    }
    else if (first_scalar)
    {
        Affine result;
        for (size_t row = 0; row < parameter.rows(); row++)
        {
            std::vector<AffineExpression> expression_row;
            for (size_t col = 0; col < variable.cols(); col++)
            {
                expression_row.emplace_back(AffineTerm(parameter(0), variable(row, col)));
            }
            result.expressions.push_back(expression_row);
        }
        return result;
    }
    else if (second_scalar)
    {
        Affine result;
        for (size_t row = 0; row < parameter.rows(); row++)
        {
            std::vector<AffineExpression> expression_row;
            for (size_t col = 0; col < variable.cols(); col++)
            {
                expression_row.emplace_back(AffineTerm(parameter(row, col), variable(0)));
            }
            result.expressions.push_back(expression_row);
        }
        return result;
    }
    else
    {
        throw std::runtime_error("Something went very wrong.");
    }
}

std::ostream &operator<<(std::ostream &os, const Affine &expression)
{
    for (size_t row = 0; row < expression.rows(); row++)
    {
        os << "[";
        for (size_t col = 0; col < expression.cols(); col++)
        {
            os << expression(row, col);
            if (col != expression.cols() - 1)
            {
                os << ", ";
            }
        }
        os << "]\n";
    }
    return os;
}

Norm2::Norm2(const Affine &affine)
{
    assert(affine.rows() == 1 or affine.cols() == 1);

    if (affine.rows() == 1)
    {
        arguments = affine.expressions.front();
    }
    else
    {
        for (const auto &row : affine.expressions)
        {
            arguments.push_back(row.front());
        }
    }
}

double Norm2::evaluate(const std::vector<double> &soln_values) const
{
    auto sum_squares = [&soln_values](double sum, const auto &arg) {
        double val = arg.evaluate(soln_values);
        return val * val;
    };
    return std::sqrt(std::accumulate(arguments.begin(), arguments.end(), 0., sum_squares));
}

} // namespace op