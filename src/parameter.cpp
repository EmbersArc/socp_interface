#include "parameter.hpp"

#include <sstream>
#include <iostream>
#include <cassert>

namespace op
{

ValueSource::ValueSource(const double const_value)
    : source(const_value) {}

ValueSource::ValueSource(double *value_ptr)
    : source(value_ptr) {}

ValueSource::ValueSource(std::function<double()> callback)
    : source(callback) {}

double ValueSource::getValue() const
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
    : source_matrix({{ValueSource(const_value)}}) {}

Parameter::Parameter(double *value_ptr)
    : source_matrix({{ValueSource(value_ptr)}}) {}

Parameter::Parameter(const std::vector<std::vector<ValueSource>> &sources)
    : source_matrix(sources) {}

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

double Parameter::getValue(const size_t row, const size_t col) const
{
    return source_matrix[row][col].getValue();
}

Parameter Parameter::operator+(const Parameter &other) const
{
    assert(shape() == other.shape());

    std::vector<std::vector<ValueSource>> sources;
    for (size_t row = 0; row < rows(); row++)
    {
        std::vector<ValueSource> result_row;
        for (size_t col = 0; col < cols(); col++)
        {
            auto add_op = [*this, other, row, col]() { return getValue(row, col) + other.getValue(row, col); };
            result_row.emplace_back(add_op);
        }
        sources.push_back(result_row);
    }
    return Parameter(sources);
}

Parameter Parameter::operator-(const Parameter &other) const
{
    assert(shape() == other.shape());

    std::vector<std::vector<ValueSource>> sources;
    for (size_t row = 0; row < rows(); row++)
    {
        std::vector<ValueSource> result_row;
        for (size_t col = 0; col < cols(); col++)
        {
            auto subtract_op = [=]() { return getValue(row, col) - other.getValue(row, col); };
            result_row.emplace_back(subtract_op);
        }
        sources.push_back(result_row);
    }
    return Parameter(sources);
}

Parameter Parameter::operator*(const Parameter &other) const
{
    bool first_matrix = rows() > 1 or cols() > 1;
    bool second_matrix = other.rows() > 1 or other.cols() > 1;
    bool both_matrix = first_matrix and second_matrix;
    if (both_matrix)
    {
        assert(cols() == other.rows());
    }

    std::vector<std::vector<ValueSource>> sources;
    for (size_t row = 0; row < rows(); row++)
    {
        std::vector<ValueSource> result_row;
        for (size_t col = 0; col < cols(); col++)
        {
            auto sum_opt = [=]() {
                double element = 0;
                for (size_t inner = 0; inner < cols(); inner++)
                {
                    element += getValue(inner, row) * other.getValue(row, inner);
                }
                return element;
            };
            result_row.emplace_back(sum_opt);
        }
        sources.push_back(result_row);
    }
    return Parameter(sources);
}

Parameter Parameter::operator/(const Parameter &other) const
{
    assert(other.rows() == 1 and other.cols() == 1);

    std::vector<std::vector<ValueSource>> sources;
    for (size_t row = 0; row < rows(); row++)
    {
        std::vector<ValueSource> result_row;
        for (size_t col = 0; col < cols(); col++)
        {
            auto divide_op = [=]() { return getValue(row, col) / other.getValue(); };
            result_row.emplace_back(divide_op);
        }
        sources.push_back(result_row);
    }
    return Parameter(sources);
}

} // namespace op