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
    return AffineTerm(ParameterSource(const_parameter), variable);
}

ParameterSource::operator AffineTerm() const
{
    return AffineTerm(*this);
}

VariableSource::operator AffineTerm() const
{
    return AffineTerm(*this);
}

AffineSum::AffineSum(const ParameterSource &parameter) : terms{parameter} {}
AffineSum::AffineSum(const VariableSource &variable) : terms{variable} {}
AffineSum::AffineSum(const AffineTerm &term) : terms{term} {}
std::ostream &operator<<(std::ostream &os, const AffineSum &expression)
{
    for (size_t i = 0; i < expression.terms.size(); i++)
    {
        if (i > 0)
            os << " + ";
        os << expression.terms[i];
    }
    return os;
}
double AffineSum::evaluate(const std::vector<double> &soln_values) const
{
    double sum = 0.;
    for (const auto &term : terms)
    {
        sum += term.evaluate(soln_values);
    }
    return sum;
}

AffineSum operator+(const AffineSum &lhs, const AffineSum &rhs)
{
    AffineSum result;
    result.terms.insert(result.terms.end(), lhs.terms.begin(), lhs.terms.end());
    result.terms.insert(result.terms.end(), rhs.terms.begin(), rhs.terms.end());
    return result;
}

AffineSum operator+(const AffineSum &lhs, const double &rhs)
{
    AffineSum result;
    result.terms.insert(result.terms.end(), lhs.terms.begin(), lhs.terms.end());
    result.terms.push_back(ParameterSource(rhs));
    return result;
}

AffineSum operator+(const double &lhs, const AffineSum &rhs)
{
    return rhs + lhs;
}

AffineSum &AffineSum::operator+=(const AffineSum &other)
{
    terms.insert(terms.end(), other.terms.begin(), other.terms.end());
    return *this;
}

ParameterSource::operator AffineSum() const
{
    return AffineSum(*this);
}

VariableSource::operator AffineSum() const
{
    return AffineSum(*this);
}

AffineTerm::operator AffineSum() const
{
    return AffineSum(*this);
}

AffineExpression::AffineExpression(const Parameter &parameter)
{
    resize(parameter.rows(), parameter.cols());
    for (size_t row = 0; row < parameter.rows(); row++)
    {
        for (size_t col = 0; col < parameter.cols(); col++)
        {
            coeffRef(row, col) = AffineSum(parameter.coeff(row, col));
        }
    }
}

AffineExpression::AffineExpression(const AffineSum &expression)
{
    resize(1, 1);
    coeffRef(0) = expression;
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

AffineExpression operator+(const AffineExpression &lhs, const AffineExpression &rhs)
{
    assert(lhs.shape() == rhs.shape());
    AffineExpression result;
    result.data_matrix = lhs.data_matrix;
    for (size_t row = 0; row < lhs.rows(); row++)
    {
        for (size_t col = 0; col < lhs.cols(); col++)
        {
            result.coeffRef(row, col) += rhs.coeff(row, col);
        }
    }
    return result;
}

AffineExpression operator*(const Parameter &parameter, const Variable &variable)
{
    if (parameter.is_scalar() and variable.is_scalar())
    {
        return AffineExpression(parameter.coeff(0) * variable.coeff(0));
    }
    else if (!parameter.is_scalar() and !variable.is_scalar())
    {
        assert(parameter.cols() == variable.rows());

        AffineExpression result(parameter.rows(), variable.cols());
        for (size_t row = 0; row < result.rows(); row++)
        {
            for (size_t col = 0; col < result.cols(); col++)
            {
                AffineSum expression;
                for (size_t inner = 0; inner < parameter.cols(); inner++)
                {
                    expression.terms.emplace_back(parameter.coeff(row, inner),
                                                  variable.coeff(inner, col));
                }
                result.coeffRef(row, col) = expression;
            }
        }
        return result;
    }
    else if (parameter.is_scalar())
    {
        AffineExpression result(variable.rows(), variable.cols());
        for (size_t row = 0; row < result.rows(); row++)
        {
            for (size_t col = 0; col < result.cols(); col++)
            {
                result.coeffRef(row, col) = AffineTerm(parameter.coeff(0),
                                                       variable.coeff(row, col));
            }
        }
        return result;
    }
    else // if (variable.is_scalar())
    {
        AffineExpression result(parameter.rows(), parameter.cols());
        for (size_t row = 0; row < result.rows(); row++)
        {
            for (size_t col = 0; col < result.cols(); col++)
            {
                result.coeffRef(row, col) = AffineTerm(parameter.coeff(row, col),
                                                       variable.coeff(0));
            }
        }
        return result;
    }
}

AffineExpression operator*(const Variable &variable, const Parameter &parameter)
{
    if (variable.is_scalar() and parameter.is_scalar())
    {
        return AffineExpression(parameter.coeff(0) * variable.coeff(0));
    }
    else if (!variable.is_scalar() and !parameter.is_scalar())
    {
        assert(variable.cols() == parameter.rows());

        AffineExpression result(variable.rows(), parameter.cols());
        for (size_t row = 0; row < result.rows(); row++)
        {
            for (size_t col = 0; col < result.cols(); col++)
            {
                AffineSum expression;
                for (size_t inner = 0; inner < variable.cols(); inner++)
                {
                    expression.terms.emplace_back(parameter.coeff(inner, col),
                                                  variable.coeff(row, inner));
                }
                result.coeffRef(row, col) = expression;
            }
        }
        return result;
    }
    else if (variable.is_scalar())
    {
        AffineExpression result(variable.rows(), parameter.cols());
        for (size_t row = 0; row < result.rows(); row++)
        {
            for (size_t col = 0; col < result.cols(); col++)
            {
                result.coeffRef(row, col) = AffineTerm(parameter.coeff(row, col),
                                                       variable.coeff(0));
            }
        }
        return result;
    }
    else // if (parameter.is_scalar())
    {
        AffineExpression result(parameter.rows(), parameter.cols());
        for (size_t row = 0; row < result.rows(); row++)
        {
            for (size_t col = 0; col < result.cols(); col++)
            {
                result.coeffRef(row, col) = AffineTerm(parameter.coeff(0), variable.coeff(row, col));
            }
        }
        return result;
    }
}

std::ostream &operator<<(std::ostream &os, const AffineExpression &expression)
{
    for (size_t row = 0; row < expression.rows(); row++)
    {
        os << "[";
        for (size_t col = 0; col < expression.cols(); col++)
        {
            os << expression.coeff(row, col);
            if (col < expression.cols() - 1)
            {
                os << ", ";
            }
        }
        os << "]\n";
    }
    return os;
}

Parameter::operator AffineExpression() const
{
    return AffineExpression(*this);
}

Norm2::Norm2(const AffineExpression &affine)
{
    assert(affine.rows() == 1 or affine.cols() == 1);

    if (affine.rows() == 1)
    {
        arguments = affine.rowElements(0);
    }
    else
    {
        arguments = affine.colElements(0);
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