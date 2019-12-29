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

AffineExpression &AffineExpression::operator+=(const AffineExpression &other)
{
    terms.insert(terms.end(), other.terms.begin(), other.terms.end());
    return *this;
}

Affine::Affine(const Parameter &parameter)
{
    resize(parameter.rows(), parameter.cols());
    for (size_t row = 0; row < parameter.rows(); row++)
    {
        for (size_t col = 0; col < parameter.cols(); col++)
        {
            coeffRef(row, col) = AffineExpression(parameter(row, col));
        }
    }
}

Affine::Affine(const AffineExpression &expression)
{
    resize(1, 1);
    coeffRef(0, 0) = expression;
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
    result.data_matrix = lhs.data_matrix;
    for (size_t row = 0; row < lhs.rows(); row++)
    {
        for (size_t col = 0; col < lhs.cols(); col++)
        {
            result(row, col) += rhs(row, col);
        }
    }
    return result;
}

Affine operator*(const Parameter &parameter, const Variable &variable)
{
    if (parameter.is_scalar() and variable.is_scalar())
    {
        return Affine(parameter(0) * variable(0));
    }
    else if (!parameter.is_scalar() and !variable.is_scalar())
    {
        assert(parameter.cols() == variable.rows());

        Affine result(parameter.rows(), variable.cols());
        for (size_t row = 0; row < result.rows(); row++)
        {
            for (size_t col = 0; col < result.cols(); col++)
            {
                AffineExpression expression;
                for (size_t inner = 0; inner < parameter.cols(); inner++)
                {
                    expression.terms.emplace_back(parameter(row, inner), variable(inner, col));
                }
                result(row, col) = expression;
            }
        }
        return result;
    }
    else if (parameter.is_scalar())
    {
        Affine result(parameter.rows(), variable.cols());
        for (size_t row = 0; row < result.rows(); row++)
        {
            for (size_t col = 0; col < result.cols(); col++)
            {
                result(row, col) = AffineTerm(parameter(0), variable(row, col));
            }
        }
        return result;
    }
    else // if (variable.is_scalar())
    {
        Affine result(parameter.rows(), variable.cols());
        for (size_t row = 0; row < result.rows(); row++)
        {
            for (size_t col = 0; col < result.cols(); col++)
            {
                result(row, col) = AffineTerm(parameter(row, col), variable(0));
            }
        }
        return result;
    }
}

Affine operator*(const Variable &variable, const Parameter &parameter)
{
    if (variable.is_scalar() and parameter.is_scalar())
    {
        return Affine(parameter(0) * variable(0));
    }
    else if (!variable.is_scalar() and !parameter.is_scalar())
    {
        assert(variable.cols() == parameter.rows());

        Affine result(variable.rows(), parameter.cols());
        for (size_t row = 0; row < result.rows(); row++)
        {
            for (size_t col = 0; col < result.cols(); col++)
            {
                AffineExpression expression;
                for (size_t inner = 0; inner < variable.cols(); inner++)
                {
                    expression.terms.emplace_back(parameter(inner, col), variable(row, inner));
                }
                result(row, col) = expression;
            }
        }
        return result;
    }
    else if (variable.is_scalar())
    {
        Affine result(variable.rows(), parameter.cols());
        for (size_t row = 0; row < result.rows(); row++)
        {
            for (size_t col = 0; col < result.cols(); col++)
            {
                result(row, col) = AffineTerm(parameter(row, col), variable(0, 0));
            }
        }
        return result;
    }
    else // if (parameter.is_scalar())
    {
        Affine result(parameter.rows(), parameter.cols());
        for (size_t row = 0; row < result.rows(); row++)
        {
            for (size_t col = 0; col < result.cols(); col++)
            {
                result(row, col) = AffineTerm(parameter(0), variable(row, col));
            }
        }
        return result;
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
            if (col < expression.cols() - 1)
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
        arguments = affine.row(0);
    }
    else
    {
        arguments = affine.col(0);
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