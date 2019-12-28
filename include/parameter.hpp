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

using double_vector_t = std::vector<double>;
using double_matrix_t = std::vector<double_vector_t>;

class ParameterSource
{
public:
    ParameterSource() = default;
    ParameterSource(const double const_value);
    explicit ParameterSource(double *value_ptr);
    explicit ParameterSource(std::function<double()> callback);

    double getValue() const;

private:
    using source_variant_t = std::variant<double,
                                          const double *,
                                          std::function<double()>>;
    source_variant_t source;
};

using parameter_source_ptr_t = std::shared_ptr<ParameterSource>;
using parameter_source_vector_t = std::vector<ParameterSource>;
using parameter_source_matrix_t = std::vector<parameter_source_vector_t>;

class Parameter
{
public:
    explicit Parameter(const double const_value);
    explicit Parameter(double *value_ptr);
    explicit Parameter(const parameter_source_matrix_t &sources);

#if EIGEN_AVAILABLE
    template <typename Derived>
    explicit Parameter(const Eigen::MatrixBase<Derived> &matrix);
    template <typename Derived>
    explicit Parameter(Eigen::MatrixBase<Derived> *matrix);
#endif

    size_t rows() const;
    size_t cols() const;
    std::pair<size_t, size_t> shape() const;
    Parameter operator+(const Parameter &other) const;
    Parameter operator-(const Parameter &other) const;
    Parameter operator*(const Parameter &other) const;
    Parameter operator/(const Parameter &other) const;
    ParameterSource operator()(const size_t row = 0,
                               const size_t col = 0) const;
    double getValue(const size_t row = 0,
                    const size_t col = 0) const;
    std::vector<std::vector<double>> getValues() const;

private:
    parameter_source_matrix_t source_matrix;
};

#ifdef EIGEN_AVAILABLE
template <typename Derived>
Parameter::Parameter(const Eigen::MatrixBase<Derived> &matrix)
{
    for (size_t row = 0; row < size_t(matrix.rows()); row++)
    {
        parameter_source_vector_t result_row;
        for (size_t col = 0; col < size_t(matrix.cols()); col++)
        {
            result_row.emplace_back(matrix.coeff(row, col));
        }
        this->source_matrix.push_back(result_row);
    }
}
template <typename Derived>
Parameter::Parameter(Eigen::MatrixBase<Derived> *matrix)
{
    for (size_t row = 0; row < size_t(matrix->rows()); row++)
    {
        parameter_source_vector_t result_row;
        for (size_t col = 0; col < size_t(matrix->cols()); col++)
        {
            result_row.emplace_back(&matrix->coeffRef(row, col));
        }
        this->source_matrix.push_back(result_row);
    }
}
#endif

} // namespace op