#include "parameter.hpp"

#include <sstream>
#include <iostream>
#include <cassert>

namespace op
{

Parameter::Parameter() : source(0), sourceType(ParameterSourceType::Constant) {}

Parameter::Parameter(const double const_value)
    : source(const_value), sourceType(ParameterSourceType::Constant) {}

Parameter::Parameter(const double *dynamic_value_ptr)
    : source(dynamic_value_ptr), sourceType(ParameterSourceType::Pointer)
{
    if (dynamic_value_ptr == nullptr)
        throw std::runtime_error("Parameter(nullptr), Null Pointer Error");
}

Parameter::Parameter(std::function<double()> callback)
    : source(callback), sourceType(ParameterSourceType::Callback)
{
    if (!callback)
        throw std::runtime_error("Parameter(callback), Invalid Callback Error");
}

Parameter::~Parameter() {}

double Parameter::get_value() const
{
    switch (sourceType)
    {
    case ParameterSourceType::Constant:
        return std::get<0>(source);
    case ParameterSourceType::Pointer:
        return *std::get<1>(source);
    default: // case ParameterSourceType::Callback:
        return std::get<2>(source)();
    }
}

std::ostream &operator<<(std::ostream &os, const Parameter &parameter)
{
    os << "(" << parameter.get_value() << ")";
    return os;
}

Parameter Parameter::operator+(const Parameter &other) const
{
    return Parameter([*this, other]() { return this->get_value() + other.get_value(); });
}

Parameter Parameter::operator-(const Parameter &other) const
{
    return Parameter([*this, other]() { return this->get_value() - other.get_value(); });
}

Parameter Parameter::operator-() const
{
    return Parameter([*this]() { return -this->get_value(); });
}

Parameter Parameter::operator*(const Parameter &other) const
{
    return Parameter([*this, other]() { return this->get_value() * other.get_value(); });
}

Parameter Parameter::operator/(const Parameter &other) const
{
    return Parameter([*this, other]() { return this->get_value() / other.get_value(); });
}

ParameterMatrix::ParameterMatrix(const std::vector<std::vector<Parameter>> &matrix)
{
    this->matrix = matrix;
}

size_t ParameterMatrix::rows() const
{
    return matrix.size();
}

size_t ParameterMatrix::cols() const
{
    return matrix.front().size();
}

std::pair<size_t, size_t> ParameterMatrix::shape() const
{
    return {rows(), cols()};
}

Parameter ParameterMatrix::operator()(const size_t row, const size_t col) const
{
    return matrix[row][col];
}

ParameterMatrix ParameterMatrix::operator+(const ParameterMatrix &other) const
{
    assert(shape() == other.shape());
    std::vector<std::vector<Parameter>> result;
    for (size_t row = 0; row < rows(); row++)
    {
        std::vector<Parameter> result_row;
        for (size_t col = 0; col < cols(); col++)
        {
            result_row.push_back(operator()(row, col) + other(row, col));
        }
        result.push_back(result_row);
    }
    return ParameterMatrix(result);
}

ParameterMatrix ParameterMatrix::operator-(const ParameterMatrix &other) const
{
    assert(shape() == other.shape());
    std::vector<std::vector<Parameter>> result;
    for (size_t row = 0; row < rows(); row++)
    {
        std::vector<Parameter> result_row;
        for (size_t col = 0; col < cols(); col++)
        {
            result_row.push_back(operator()(row, col) - other(row, col));
        }
        result.push_back(result_row);
    }
    return ParameterMatrix(result);
}

ParameterMatrix ParameterMatrix::operator*(const ParameterMatrix &other) const
{
    assert(cols() == other.rows());
    std::vector<std::vector<Parameter>> result;
    for (size_t row = 0; row < rows(); row++)
    {
        std::vector<Parameter> result_row;
        for (size_t col = 0; col < other.cols(); col++)
        {
            auto sum_operation = [row, col, this, &other]() {
                double result = 0;
                for (size_t inner = 0; inner < cols(); inner++)
                {
                    result += this->operator()(row, inner).get_value() *
                              other(inner, col).get_value();
                }
                return result;
            };
            result_row.emplace_back(sum_operation);
        }
        result.push_back(result_row);
    }
    return ParameterMatrix(result);
}

} // namespace op