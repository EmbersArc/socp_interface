#include "parameter.hpp"

#include <sstream>
#include <cassert>

namespace op
{

ParameterSource::ParameterSource(const double const_value)
    : source(const_value) {}

ParameterSource::ParameterSource(double *value_ptr)
    : source(value_ptr) {}

ParameterSource::ParameterSource(const std::function<double()> &callback)
    : source(callback) {}

double ParameterSource::getValue() const
{
    switch (source.index())
    {
    case 0:
        return std::get<0>(source);
    case 1:
        return *std::get<1>(source);
    default:
        return std::get<2>(source)();
    }
}

ParameterSource operator+(const ParameterSource &parameter1,
                          const ParameterSource &parameter2)
{
    auto add_op = [p1 = parameter1,
                   p2 = parameter2]() {
        return p1.getValue() + p2.getValue();
    };
    return ParameterSource(add_op);
}

ParameterSource operator-(const ParameterSource &parameter1,
                          const ParameterSource &parameter2)
{
    auto subtract_op = [p1 = parameter1,
                        p2 = parameter2]() {
        return p1.getValue() - p2.getValue();
    };
    return ParameterSource(subtract_op);
}

ParameterSource operator*(const ParameterSource &parameter1,
                          const ParameterSource &parameter2)
{
    auto multiply_op = [p1 = parameter1,
                        p2 = parameter2]() {
        return p1.getValue() * p2.getValue();
    };
    return ParameterSource(multiply_op);
}

ParameterSource operator/(const ParameterSource &parameter1,
                          const ParameterSource &parameter2)
{
    auto divide_op = [p1 = parameter1,
                      p2 = parameter2]() {
        return p1.getValue() / p2.getValue();
    };
    return ParameterSource(divide_op);
}

Parameter::Parameter(const double const_value)
{
    data_matrix = {{ParameterSource(const_value)}};
}

Parameter::Parameter(double *value_ptr)
{
    data_matrix = {{ParameterSource(value_ptr)}};
}

Parameter::Parameter(const std::function<double()> &callback)
{
    data_matrix = {{ParameterSource(callback)}};
}

Parameter::Parameter(const parameter_source_matrix_t &sources)
{
    assert(not sources.empty());
    data_matrix = sources;
}

double Parameter::getValue(const size_t row, const size_t col) const
{
    return coeff(row, col).getValue();
}

DynamicMatrix<double> Parameter::getValues() const
{
    DynamicMatrix<double> result_matrix(rows(), cols());

    for (size_t row = 0; row < rows(); row++)
    {
        for (size_t col = 0; col < cols(); col++)
        {
            result_matrix.coeffRef(row, col) = getValue(row, col);
        }
    }
    return result_matrix;
}

Parameter Parameter::operator+(const Parameter &other) const
{
    assert(shape() == other.shape());

    Parameter parameter(rows(), cols());
    for (size_t row = 0; row < rows(); row++)
    {
        for (size_t col = 0; col < cols(); col++)
        {
            parameter.coeffRef(row, col) = coeff(row, col) + other.coeff(row, col);
        }
    }
    return parameter;
}

Parameter Parameter::operator-() const
{
    return Parameter(-1.0) * *this;
}

Parameter Parameter::operator-(const Parameter &other) const
{
    assert(shape() == other.shape());

    Parameter parameter(rows(), cols());
    for (size_t row = 0; row < rows(); row++)
    {
        for (size_t col = 0; col < cols(); col++)
        {
            parameter.coeffRef(row, col) = coeff(row, col) - other.coeff(row, col);
        }
    }
    return parameter;
}

Parameter scaleMatrix(const ParameterSource &scalar, const Parameter &matrix)
{
    Parameter parameter(matrix.rows(), matrix.cols());
    for (size_t row = 0; row < matrix.rows(); row++)
    {
        for (size_t col = 0; col < matrix.cols(); col++)
        {
            parameter.coeffRef(row, col) = scalar * matrix.coeff(row, col);
        }
    }
    return parameter;
}

Parameter multiplyMatrices(const Parameter &matrix1, const Parameter &matrix2)
{
    assert(matrix1.cols() == matrix2.rows());

    Parameter parameter(matrix1.rows(), matrix2.cols());
    for (size_t row = 0; row < matrix1.rows(); row++)
    {
        for (size_t col = 0; col < matrix2.cols(); col++)
        {
            std::vector<ParameterSource> row_vector(matrix1.cols());
            std::vector<ParameterSource> column_vector(matrix2.rows());
            for (size_t inner = 0; inner < matrix1.cols(); inner++)
            {
                row_vector.at(inner) = matrix1.coeff(row, inner);
                column_vector.at(inner) = matrix2.coeff(inner, col);
            }
            auto sum_op = [rv = row_vector, cv = column_vector] {
                double element = 0;
                for (size_t inner = 0; inner < rv.size(); inner++)
                {
                    element += rv[inner].getValue() * cv[inner].getValue();
                }
                return element;
            };
            parameter.coeffRef(row, col) = ParameterSource(sum_op);
        }
    }
    return parameter;
}

Parameter Parameter::operator*(const Parameter &other) const
{
    if (is_scalar() and other.is_scalar())
    {
        return Parameter({{coeff(0) * other.coeff(0)}});
    }
    if (!is_scalar() and !other.is_scalar())
    {
        return multiplyMatrices(*this, other);
    }
    if (is_scalar())
    {
        return scaleMatrix(coeff(0), other);
    }
    else // if (other.is_scalar())
    {
        return scaleMatrix(other.coeff(0), *this);
    }
}

Parameter Parameter::operator/(const Parameter &other) const
{
    assert(other.is_scalar());

    Parameter parameter(rows(), cols());
    for (size_t row = 0; row < rows(); row++)
    {
        for (size_t col = 0; col < cols(); col++)
        {
            parameter.coeffRef(row, col) = coeff(row, col) / other.coeff(0);
        }
    }
    return parameter;
}

} // namespace op