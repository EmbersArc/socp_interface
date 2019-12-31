#pragma once

#include <functional>
#include <string>
#include <utility>
#include <variant>
#include <memory>
#include <iostream>

#include "dynamicMatrix.hpp"

// #ifdef EIGEN_AVAILABLE
#include <Eigen/Dense>
// #endif

namespace op
{

struct AffineTerm;
struct AffineSum;
class AffineExpression;

class ParameterSource
{
public:
    ParameterSource() = default;
    ParameterSource(const double const_value);
    explicit ParameterSource(double *value_ptr);
    explicit ParameterSource(const std::function<double()> &callback);
    double getValue() const;

    operator AffineTerm() const;
    operator AffineSum() const;

private:
    using source_variant_t = std::variant<double,
                                          const double *,
                                          std::function<double()>>;
    source_variant_t source;
};

using parameter_source_ptr_t = std::shared_ptr<ParameterSource>;
using parameter_source_vector_t = std::vector<ParameterSource>;
using parameter_source_matrix_t = std::vector<parameter_source_vector_t>;

class Parameter : public DynamicMatrix<ParameterSource>
{
public:
    using DynamicMatrix<ParameterSource>::DynamicMatrix;
    explicit Parameter(const double const_value);
    explicit Parameter(double *value_ptr);
    explicit Parameter(const parameter_source_matrix_t &sources);

    // #if EIGEN_AVAILABLE
    template <typename Derived>
    explicit Parameter(const Eigen::DenseBase<Derived> &matrix);
    template <typename Derived>
    explicit Parameter(Eigen::DenseBase<Derived> *matrix);
    // #endif

    Parameter operator+(const Parameter &other) const;
    Parameter operator-() const;
    Parameter operator-(const Parameter &other) const;
    Parameter operator*(const Parameter &other) const;
    Parameter operator/(const Parameter &other) const;
    double getValue(const size_t row = 0,
                    const size_t col = 0) const;
    DynamicMatrix<double> getValues() const;

    operator AffineExpression() const;
};

// #ifdef EIGEN_AVAILABLE
template <typename Derived>
Parameter::Parameter(const Eigen::DenseBase<Derived> &matrix)
{
    resize(matrix.rows(), matrix.cols());
    for (size_t row = 0; row < size_t(matrix.rows()); row++)
    {
        for (size_t col = 0; col < size_t(matrix.cols()); col++)
        {
            coeffRef(row, col) = ParameterSource(matrix(row, col));
        }
    }
}
template <typename Derived>
Parameter::Parameter(Eigen::DenseBase<Derived> *matrix)
{
    resize(matrix->rows(), matrix->cols());
    for (size_t row = 0; row < size_t(matrix->rows()); row++)
    {
        for (size_t col = 0; col < size_t(matrix->cols()); col++)
        {
            coeffRef(row, col) = ParameterSource(&matrix->coeffRef(row, col));
        }
    }
}
// #endif

} // namespace op