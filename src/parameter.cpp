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

Parameter::Parameter(const double const_value)
    : source_matrix({{ParameterSource(const_value)}}) {}

Parameter::Parameter(double *value_ptr)
    : source_matrix({{ParameterSource(value_ptr)}}) {}

Parameter::Parameter(const parameter_source_matrix_t &sources)
    : source_matrix(sources)
{
    assert(not sources.empty());
}

size_t Parameter::rows() const
{
    return source_matrix.size();
}

size_t Parameter::cols() const
{
    return source_matrix.front().size();
}

std::pair<size_t, size_t> Parameter::shape() const
{
    return {rows(), cols()};
}

ParameterSource Parameter::operator()(const size_t row, const size_t col) const
{
    assert(not(rows() == 0 or cols() == 0));
    assert(row < rows() and col < cols());
    return source_matrix.at(row).at(col);
}

double Parameter::getValue(const size_t row, const size_t col) const
{
    return operator()(row, col).getValue();
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
    parameter_source_matrix_t sources;
    for (size_t row = 0; row < rows(); row++)
    {
        parameter_source_vector_t result_row;
        for (size_t col = 0; col < cols(); col++)
        {
            auto var1 = std::make_shared<ParameterSource>();
            *var1 = operator()(row, col);
            auto var2 = std::make_shared<ParameterSource>();
            *var2 = other(row, col);
            auto add_op = [=]() { return var1->getValue() + var2->getValue(); };
            result_row.emplace_back(add_op);
        }
        sources.push_back(result_row);
    }
    return Parameter(sources);
}

Parameter Parameter::operator-(const Parameter &other) const
{
    assert(shape() == other.shape());

    parameter_source_matrix_t sources;
    for (size_t row = 0; row < rows(); row++)
    {
        parameter_source_vector_t result_row;
        for (size_t col = 0; col < cols(); col++)
        {
            auto var1 = std::make_shared<ParameterSource>();
            *var1 = operator()(row, col);
            auto var2 = std::make_shared<ParameterSource>();
            *var2 = other(row, col);
            auto subtract_op = [=]() { return var1->getValue() - var2->getValue(); };
            result_row.emplace_back(subtract_op);
        }
        sources.push_back(result_row);
    }
    return Parameter(sources);
}

parameter_source_matrix_t multiplyScalars(const ParameterSource &scalar1, const ParameterSource &scalar2)
{
    auto var1 = std::make_shared<ParameterSource>();
    *var1 = scalar1;
    auto var2 = std::make_shared<ParameterSource>();
    *var2 = scalar2;
    auto multiply_op = [=]() { return var1->getValue() * var2->getValue(); };

    return {{ParameterSource(multiply_op)}};
}

parameter_source_matrix_t scaleMatrix(const ParameterSource &scalar, const Parameter &matrix)
{
    parameter_source_matrix_t sources;
    for (size_t row = 0; row < matrix.rows(); row++)
    {
        parameter_source_vector_t result_row;
        for (size_t col = 0; col < matrix.cols(); col++)
        {
            auto var1 = std::make_shared<ParameterSource>();
            *var1 = scalar;
            auto var2 = std::make_shared<ParameterSource>();
            *var2 = matrix(row, col);
            auto multiply_op = [=]() { return var1->getValue() * var2->getValue(); };
            result_row.emplace_back(multiply_op);
        }
        sources.push_back(result_row);
    }
    return sources;
}

parameter_source_matrix_t multiplyMatrices(const Parameter &matrix1, const Parameter &matrix2)
{
    parameter_source_matrix_t sources;
    for (size_t row = 0; row < matrix1.rows(); row++)
    {
        parameter_source_vector_t result_row;
        for (size_t col = 0; col < matrix1.cols(); col++)
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
            result_row.emplace_back(sum_opt);
        }
        sources.push_back(result_row);
    }
    return sources;
}

Parameter Parameter::operator*(const Parameter &other) const
{
    bool first_scalar = rows() == 1 and cols() == 1;
    bool second_scalar = other.rows() == 1 and other.cols() == 1;

    bool both_scalar = first_scalar and second_scalar;
    bool both_matrix = not first_scalar and not second_scalar;

    parameter_source_matrix_t sources;

    if (both_scalar)
    {
        sources = multiplyScalars(operator()(), other());
    }
    if (both_matrix)
    {
        sources = multiplyMatrices(*this, other);
    }
    if (first_scalar)
    {
        sources = scaleMatrix(operator()(), other);
    }
    else if (second_scalar)
    {
        sources = scaleMatrix(other(), *this);
    }

    return Parameter(sources);
}

Parameter Parameter::operator/(const Parameter &other) const
{
    assert(other.rows() == 1 and other.cols() == 1);

    parameter_source_matrix_t sources;
    for (size_t row = 0; row < rows(); row++)
    {
        parameter_source_vector_t result_row;
        for (size_t col = 0; col < cols(); col++)
        {
            auto var1 = std::make_shared<ParameterSource>();
            *var1 = operator()(row, col);
            auto var2 = std::make_shared<ParameterSource>();
            *var2 = other(row, col);
            auto divide_op = [=]() { return var1->getValue() / var2->getValue(); };
            result_row.emplace_back(divide_op);
        }
        sources.push_back(result_row);
    }
    return Parameter(sources);
}

} // namespace op