#include "expression.hpp"

#include <sstream>
#include <numeric>
#include <cmath>

namespace op
{

AffineTerm::AffineTerm(const Parameter &parameter) : parameter(parameter) {}

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
    AffineTerm affineTerm;
    affineTerm.parameter = parameter;
    affineTerm.variable = variable;
    return affineTerm;
}

AffineTerm operator*(const Variable &variable, const Parameter &parameter)
{
    return parameter * variable;
}

AffineTerm operator*(const double &const_parameter, const Variable &variable)
{
    AffineTerm affineTerm;
    affineTerm.parameter = Parameter(const_parameter);
    affineTerm.variable = variable;
    return affineTerm;
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

Norm2::Norm2(const std::vector<AffineExpression> &affineExpressions)
    : arguments(affineExpressions) {}

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

} // namespace op