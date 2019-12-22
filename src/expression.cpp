#include "expression.hpp"

#include <sstream>
#include <numeric>
#include <cmath>

namespace op
{

AffineTerm::AffineTerm(const Parameter &parameter)
    : parameter(parameter) {}
AffineTerm::AffineTerm(const Variable &variable)
    : parameter(1.0), variable(variable) {}
AffineTerm::AffineTerm(const Parameter &parameter, const Variable &variable)
    : parameter(parameter), variable(variable) {}

AffineExpression::AffineExpression(const Parameter &parameter) : terms({parameter}) {}
AffineExpression::AffineExpression(const AffineTerm &term) : terms{term} {}

AffineMatrix::AffineMatrix(const AffineExpression &expression) : expressions({{expression}}) {}

std::ostream &operator<<(std::ostream &os, const AffineTerm &term)
{
    os << term.parameter;
    if (term.variable)
        os << "*" << term.variable.value();
    return os;
}

double AffineTerm::evaluate(const std::vector<double> &soln_values) const
{
    double p = parameter.get_value();
    if (variable)
    {
        return p * soln_values[variable.value().problem_index];
    }
    else
    {
        return p;
    }
}

std::ostream &operator<<(std::ostream &os, const AffineExpression &expression)
{
    for (size_t i = 0; i < expression.terms.size(); i++)
    {
        if (i > 0)
            os << "  + ";
        os << expression.terms[i];
    }
    return os;
}

double AffineExpression::evaluate(const std::vector<double> &soln_values) const
{
    auto sum_terms = [&soln_values](double sum, const auto &term) { return sum + term.evaluate(soln_values); };
    return std::accumulate(terms.begin(), terms.end(), 0., sum_terms);
}

AffineTerm operator*(const Parameter &parameter, const Variable &variable)
{
    return AffineTerm(parameter, variable);
}

AffineTerm operator*(const Variable &variable, const Parameter &parameter)
{
    return parameter * variable;
}

AffineTerm operator*(const double &const_parameter, const Variable &variable)
{
    return AffineTerm(const_parameter, variable);
}

AffineTerm operator*(const Variable &variable, const double &const_parameter)
{
    return const_parameter * variable;
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
    result.terms.push_back(Parameter(rhs));
    return result;
}

AffineExpression operator+(const double &lhs, const AffineExpression &rhs)
{
    return rhs + lhs;
}

Norm2::Norm2(const AffineMatrix &affineMatrix)
{
    assert(affineMatrix.rows() >= 1 and affineMatrix.cols() == 1);
    for (const auto &row : affineMatrix.expressions)
    {
        arguments.push_back(row.front());
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

std::ostream &operator<<(std::ostream &os, const Norm2 &norm2)
{
    os << "norm2([ ";
    for (size_t i = 0; i < norm2.arguments.size(); i++)
    {
        if (i > 0)
            os << ", ";
        os << norm2.arguments[i];
    }
    os << " ])";
    return os;
}

size_t AffineMatrix::rows() const
{
    return expressions.size();
}

size_t AffineMatrix::cols() const
{
    return expressions.front().size();
}

std::pair<size_t, size_t> AffineMatrix::shape() const
{
    return {rows(), cols()};
}

AffineExpression AffineMatrix::operator()(const size_t row, const size_t col) const
{
    return expressions[row][col];
}

AffineMatrix operator+(const AffineMatrix &lhs, const AffineMatrix &rhs)
{
    assert(lhs.shape() == rhs.shape());
    AffineMatrix result;
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

AffineMatrix operator*(const ParameterMatrix &parameter, const VariableMatrix &variable)
{
    assert(parameter.cols() == variable.rows());
    AffineMatrix result;
    for (size_t row = 0; row < parameter.rows(); row++)
    {
        std::vector<AffineExpression> expression_row;
        for (size_t col = 0; col < variable.cols(); col++)
        {
            AffineExpression expression;
            for (size_t inner = 0; inner < parameter.cols(); inner++)
            {
                AffineTerm term(parameter(row, inner), variable(inner, col));
                expression.terms.push_back(term);
            }
            expression_row.push_back(expression);
        }
        result.expressions.push_back(expression_row);
    }
    return result;
}

AffineMatrix operator+(const ParameterMatrix &parameter, const VariableMatrix &variable)
{
    assert(parameter.shape() == variable.shape());
    AffineMatrix result;
    for (size_t row = 0; row < parameter.rows(); row++)
    {
        std::vector<AffineExpression> expression_row;
        for (size_t col = 0; col < variable.cols(); col++)
        {
            AffineTerm term1(parameter(row, col));
            AffineTerm term2(1.0, variable(row, col));
            expression_row.push_back(term1 + term2);
        }
    }
    return result;
}

} // namespace op