#include "expression.hpp"

#include <sstream>
#include <numeric>
#include <cmath>

namespace op
{

AffineTerm::AffineTerm(const Parameter &parameter) : parameter(parameter) {}

AffineExpression::AffineExpression(const Parameter &parameter) : terms({parameter}) {}

AffineTerm::operator AffineExpression()
{
    AffineExpression result;
    result.terms.push_back(*this);
    return result;
}

std::ostream &operator<<(std::ostream &os, const AffineTerm &constraint)
{
    os << constraint.parameter;
    if (constraint.variable)
        os << "*" << constraint.variable.value();
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

std::ostream &operator<<(std::ostream &os, const AffineExpression &constraint)
{
    for (size_t i = 0; i < constraint.terms.size(); i++)
    {
        if (i > 0)
            os << "  + ";
        os << constraint.terms[i];
    }
    return os;
}

double AffineExpression::evaluate(const std::vector<double> &soln_values) const
{
    auto sum_terms = [&soln_values](double sum, const auto &term) { return sum + term.evaluate(soln_values); };
    return std::accumulate(terms.begin(), terms.end(), 0., sum_terms);
}

std::ostream &operator<<(std::ostream &os, const Norm2 &constraint)
{
    os << "norm2([ ";
    for (size_t i = 0; i < constraint.arguments.size(); i++)
    {
        if (i > 0)
            os << ", ";
        os << constraint.arguments[i];
    }
    os << " ])";
    return os;
}

double Norm2::evaluate(const std::vector<double> &soln_values) const
{
    auto sum_squares = [&soln_values](double sum, const auto &arg) {
        double val = arg.evaluate(soln_values);
        return val * val;
    };
    return std::sqrt(std::accumulate(arguments.begin(), arguments.end(), 0., sum_squares));
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

} // namespace op