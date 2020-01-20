#pragma once

#include <functional>
#include <string>
#include <utility>
#include <variant>
#include <memory>

#include "dynamicMatrix.hpp"

#include <Eigen/Dense>

namespace op
{

<<<<<<< HEAD
// Represents a parameter p_i in the opt-problem that can be changed between problem evaluations.
// The parameter value can either be constant or dynamically accessed through a pointer or callback.
class Parameter
{
    using parameter_variant_t = std::variant<double,
                                             const double *,
                                             std::function<double()>>;
    parameter_variant_t source;
=======
struct AffineTerm;
struct AffineSum;
class Affine;

class ParameterSource
{
public:
    ParameterSource() = default;
    ParameterSource(const double const_value);
    explicit ParameterSource(double *value_ptr);
    explicit ParameterSource(const std::function<double()> &callback);
    double get_value() const;
    bool is_constant() const;
    bool is_pointer() const;
    bool is_callback() const;
    bool is_zero() const;
    bool is_one() const;

    ParameterSource operator+(const ParameterSource &other) const;
    ParameterSource operator-(const ParameterSource &other) const;
    ParameterSource operator*(const ParameterSource &other) const;
    ParameterSource operator/(const ParameterSource &other) const;
    operator AffineTerm() const;
    operator AffineSum() const;

private:
    using source_variant_t = std::variant<double,
                                          const double *,
                                          std::function<double()>>;
    source_variant_t source;
};
>>>>>>> revamp

class Parameter : public DynamicMatrix<ParameterSource, Parameter>
{
public:
<<<<<<< HEAD
    Parameter();
    Parameter(double const_value);
    explicit Parameter(const double *dynamic_value_ptr);
    explicit Parameter(std::function<double()> callback);
    ~Parameter();
=======
    using DynamicMatrix<ParameterSource, Parameter>::DynamicMatrix;
    explicit Parameter(const double const_value);
    explicit Parameter(double *value_ptr);
    explicit Parameter(const std::function<double()> &callback);
>>>>>>> revamp

    template <typename Derived>
    explicit Parameter(const Eigen::DenseBase<Derived> &matrix);
    template <typename Derived>
    explicit Parameter(Eigen::DenseBase<Derived> *matrix);

    Parameter operator+(const Parameter &other) const;
    Parameter operator-() const;
    Parameter operator-(const Parameter &other) const;
    Parameter operator*(const Parameter &other) const;
    Parameter operator/(const Parameter &other) const;
    Affine cwiseProduct(const Affine &affine) const;
    double get_value(const size_t row = 0,
                    const size_t col = 0) const;
    Eigen::MatrixXd get_values() const;

<<<<<<< HEAD
    Parameter operator+(const Parameter &par);
    Parameter operator-(const Parameter &par);
    Parameter operator-();
    Parameter operator*(const Parameter &par);
    Parameter operator/(const Parameter &par);
};
=======
    operator Affine() const;
};

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
>>>>>>> revamp

} // namespace op