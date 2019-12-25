#pragma once

#include <functional>
#include <string>
#include <utility>
#include <variant>
#include <memory>

#ifdef EIGEN_AVAILABLE
#include <Eigen/Dense>
#endif

namespace op
{

class ValueSource
{
public:
    explicit ValueSource(const double const_value);
    explicit ValueSource(double *value_ptr);
    explicit ValueSource(std::function<double()> callback);

    double getValue() const;

private:
    using source_variant_t = std::variant<double,
                                          const double *,
                                          std::function<double()>>;
    source_variant_t source;
};

class Parameter
{
public:
    explicit Parameter(const double const_value);
    explicit Parameter(double *value_ptr);
    explicit Parameter(const std::vector<std::vector<ValueSource>> &sources);

#if EIGEN_AVAILABLE
    template <typename Derived>
    explicit Parameter(const Eigen::PlainObjectBase<Derived> &matrix);
    template <typename Derived>
    explicit Parameter(const Eigen::PlainObjectBase<Derived> *matrix);
#endif

    size_t rows() const;
    size_t cols() const;
    std::pair<size_t, size_t> shape() const;
    Parameter operator+(const Parameter &other) const;
    Parameter operator-(const Parameter &other) const;
    Parameter operator*(const Parameter &other) const;
    Parameter operator/(const Parameter &other) const;
    ValueSource operator()(const size_t row = 0, const size_t col = 0) const;
    std::vector<std::vector<double>> getValues() const;

private:
    std::vector<std::vector<ValueSource>> source_matrix;
};

#ifdef EIGEN_AVAILABLE
template <typename Derived>
Parameter::Parameter(const Eigen::PlainObjectBase<Derived> &matrix)
{
    for (size_t row = 0; row < size_t(matrix.rows()); row++)
    {
        std::vector<ValueSource> result_row;
        for (size_t col = 0; col < size_t(matrix.cols()); col++)
        {
            result_row.emplace_back(matrix.coeff(row, col));
        }
        this->source_matrix.push_back(result_row);
    }
}
template <typename Derived>
Parameter::Parameter(const Eigen::PlainObjectBase<Derived> *matrix)
{
    for (size_t row = 0; row < size_t(matrix->rows()); row++)
    {
        std::vector<ValueSource> result_row;
        for (size_t col = 0; col < size_t(matrix->cols()); col++)
        {
            result_row.emplace_back(&matrix->coeff(row, col));
        }
        this->source_matrix.push_back(result_row);
    }
}
#endif

} // namespace op