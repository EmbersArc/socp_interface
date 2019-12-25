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
    : source_matrix({{std::make_shared<ValueSource>(const_value)}}) {}

Parameter::Parameter(double *value_ptr)
    : source_matrix({{std::make_shared<ValueSource>(value_ptr)}}) {}

Parameter::Parameter(const value_source_matrix_t &sources)
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

value_source_ptr_t Parameter::operator()(const size_t row, const size_t col) const
{
    assert(not(rows() == 0 or cols() == 0));
    assert(row < rows() and col < cols());
    return source_matrix.at(row).at(col);
}

double Parameter::getValue(const size_t row, const size_t col) const
{
    return operator()(row, col)->getValue();
}

std::vector<std::vector<double>> Parameter::getValues() const
{
    std::vector<std::vector<double>> result_matrix;
    for (size_t row = 0; row < rows(); row++)
    {
        std::vector<double> result_row;
        for (size_t col = 0; col < cols(); col++)
        {
            result_row.push_back(getValue(row, col));
        }
        result_matrix.push_back(result_row);
    }
    return result_matrix;
}

Parameter Parameter::operator+(const Parameter &other) const
{
    std::cout << "Building addition..."
              << "\n";
    assert(shape() == other.shape());
    value_source_matrix_t sources;
    for (size_t row = 0; row < rows(); row++)
    {
        value_source_vector_t result_row;
        for (size_t col = 0; col < cols(); col++)
        {
            value_source_ptr_t var1 = operator()(row, col);
            value_source_ptr_t var2 = other(row, col);
            auto add_op = [=]() { return var1->getValue() + var2->getValue(); };
            result_row.push_back(std::make_shared<ValueSource>(add_op));
        }
        sources.push_back(result_row);
    }
    std::cout << "Output Matrix shape: " << sources.size() << "," << sources.front().size()
              << "\n\n";
    return Parameter(sources);
}

Parameter Parameter::operator-(const Parameter &other) const
{
    std::cout << "Building subtraction..."
              << "\n";
    assert(shape() == other.shape());

    value_source_matrix_t sources;
    for (size_t row = 0; row < rows(); row++)
    {
        value_source_vector_t result_row;
        for (size_t col = 0; col < cols(); col++)
        {
            value_source_ptr_t var1 = operator()(row, col);
            value_source_ptr_t var2 = other(row, col);
            auto subtract_op = [=]() { return var1->getValue() - var2->getValue(); };
            result_row.push_back(std::make_shared<ValueSource>(subtract_op));
        }
        sources.push_back(result_row);
    }
    std::cout << "Output Matrix shape: " << sources.size() << "," << sources.front().size()
              << "\n\n";
    return Parameter(sources);
}

Parameter Parameter::operator*(const Parameter &other) const
{
    std::cout << "Building multiplication..."
              << "\n";
    bool first_matrix = rows() > 1 or cols() > 1;
    bool second_matrix = other.rows() > 1 or other.cols() > 1;
    bool both_matrix = first_matrix and second_matrix;
    if (both_matrix)
    {
        assert(cols() == other.rows());
    }

    value_source_matrix_t sources;
    for (size_t row = 0; row < rows(); row++)
    {
        value_source_vector_t result_row;
        for (size_t col = 0; col < cols(); col++)
        {
            std::shared_ptr<value_source_vector_t> row_vector = std::make_shared<value_source_vector_t>(source_matrix[row]);
            std::shared_ptr<value_source_vector_t> column_vector;
            for (size_t inner = 0; inner < cols(); inner++)
            {
                column_vector->emplace_back(other(row, inner));
            }
            auto sum_opt = [=]() {
                double element = 0;
                for (size_t inner = 0; inner < cols(); inner++)
                {
                    element += row_vector->at(inner)->getValue() * column_vector->at(inner)->getValue();
                }
                return element;
            };
            result_row.push_back(std::make_shared<ValueSource>(sum_opt));
        }
        sources.push_back(result_row);
    }
    std::cout << "Output Matrix shape: " << sources.size() << "," << sources.front().size()
              << "\n\n";
    return Parameter(sources);
}

Parameter Parameter::operator/(const Parameter &other) const
{
    std::cout << "Building division..."
              << "\n";
    assert(other.rows() == 1 and other.cols() == 1);

    value_source_matrix_t sources;
    for (size_t row = 0; row < rows(); row++)
    {
        value_source_vector_t result_row;
        for (size_t col = 0; col < cols(); col++)
        {
            auto divide_op = [=]() { return operator()(row, col)->getValue() / other()->getValue(); };
            result_row.push_back(std::make_shared<ValueSource>(divide_op));
        }
        sources.push_back(result_row);
    }
    return Parameter(sources);
    std::cout << "Output Matrix shape: " << sources.size() << "," << sources.front().size()
              << "\n\n";
}

} // namespace op