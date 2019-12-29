#include "parameter.hpp"

#include <sstream>
#include <cassert>

namespace op
{

ParameterSource::ParameterSource(const double const_value)
    : source(const_value) {}

ParameterSource::ParameterSource(double *value_ptr)
    : source(value_ptr) {}

ParameterSource::ParameterSource(std::function<double()> callback)
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

Parameter Parameter::transpose() const
{
    Parameter transposed(cols(), rows());
    for (size_t row = 0; row < rows(); row++)
    {
        for (size_t col = 0; col < cols(); col++)
        {
            transposed(col, row) = coeff(row, col);
        }
    }
    return transposed;
}

Parameter::Parameter(const double const_value)
{
    data_matrix = {{ParameterSource(const_value)}};
}

Parameter::Parameter(double *value_ptr)
{
    data_matrix = {{ParameterSource(value_ptr)}};
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
            result_matrix(row, col) = getValue(row, col);
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
            auto var1 = std::make_shared<ParameterSource>();
            *var1 = coeff(row, col);
            auto var2 = std::make_shared<ParameterSource>();
            *var2 = other(row, col);
            auto add_op = [=]() { return var1->getValue() + var2->getValue(); };
            parameter(row, col) = ParameterSource(add_op);
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
            auto var1 = std::make_shared<ParameterSource>();
            *var1 = coeff(row, col);
            auto var2 = std::make_shared<ParameterSource>();
            *var2 = other(row, col);
            auto subtract_op = [=]() { return var1->getValue() - var2->getValue(); };
            parameter(row, col) = ParameterSource(subtract_op);
        }
    }
    return parameter;
}

Parameter multiplyScalars(const ParameterSource &scalar1, const ParameterSource &scalar2)
{
    auto var1 = std::make_shared<ParameterSource>();
    *var1 = scalar1;
    auto var2 = std::make_shared<ParameterSource>();
    *var2 = scalar2;
    auto multiply_op = [=]() { return var1->getValue() * var2->getValue(); };

    return Parameter({{ParameterSource(multiply_op)}});
}

Parameter scaleMatrix(const ParameterSource &scalar, const Parameter &matrix)
{
    Parameter parameter(matrix.rows(), matrix.cols());
    for (size_t row = 0; row < matrix.rows(); row++)
    {
        for (size_t col = 0; col < matrix.cols(); col++)
        {
            auto var1 = std::make_shared<ParameterSource>();
            *var1 = scalar;
            auto var2 = std::make_shared<ParameterSource>();
            *var2 = matrix(row, col);
            auto multiply_op = [=]() { return var1->getValue() * var2->getValue(); };
            parameter(row, col) = ParameterSource(multiply_op);
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
            auto row_vector = std::make_shared<parameter_source_vector_t>(matrix1.cols());
            auto column_vector = std::make_shared<parameter_source_vector_t>(matrix2.rows());
            for (size_t inner = 0; inner < matrix1.cols(); inner++)
            {
                row_vector->at(inner) = matrix1(row, inner);
                column_vector->at(inner) = matrix2(inner, col);
            }
            auto sum_opt = [=]() {
                double element = 0;
                for (size_t inner = 0; inner < row_vector->size(); inner++)
                {
                    element += row_vector->at(inner).getValue() * column_vector->at(inner).getValue();
                }
                return element;
            };
            parameter(row, col) = ParameterSource(sum_opt);
        }
    }
    return parameter;
}

Parameter Parameter::operator*(const Parameter &other) const
{
    if (is_scalar() and other.is_scalar())
    {
        return multiplyScalars(coeff(0), other(0));
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
        return scaleMatrix(other(0), *this);
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
            auto var1 = std::make_shared<ParameterSource>();
            *var1 = coeff(row, col);
            auto var2 = std::make_shared<ParameterSource>();
            *var2 = other(row, col);
            auto divide_op = [=]() { return var1->getValue() / var2->getValue(); };
            parameter(row, col) = ParameterSource(divide_op);
        }
    }
    return parameter;
}

Parameter vstack(std::initializer_list<Parameter> elements)
{
    Parameter stacked;
    for (const auto &e : elements)
    {
        stacked.data_matrix.insert(stacked.data_matrix.end(),
                                   e.data_matrix.begin(),
                                   e.data_matrix.end());
    }
    return stacked;
}

Parameter hstack(std::initializer_list<Parameter> elements)
{
    Parameter stacked(elements.begin()->rows(), 0);
    for (const auto &e : elements)
    {
        assert(stacked.rows() == e.rows());
        for (size_t row = 0; row < stacked.rows(); row++)
        {
            stacked.data_matrix.at(row).insert(stacked.data_matrix.at(row).begin(),
                                               e.data_matrix.at(row).begin(),
                                               e.data_matrix.at(row).end());
        }
    }
    return stacked;
}

} // namespace op