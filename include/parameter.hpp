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

// Represents a parameter p_i in the opt-problem that can be changed between problem evaluations.
// The parameter value can either be constant or dynamically accessed through a pointer or callback.
class Parameter
{
    using parameter_variant_t = std::variant<double,
                                             const double *,
                                             std::function<double()>>;
    parameter_variant_t source;

public:
    Parameter();
    Parameter(const double const_value);
    explicit Parameter(const double *dynamic_value_ptr);
    explicit Parameter(std::function<double()> callback);
    ~Parameter();

    double get_value() const;

    friend std::ostream &operator<<(std::ostream &os, const Parameter &parameter);

    Parameter operator+(const Parameter &other) const;
    Parameter operator-(const Parameter &other) const;
    Parameter operator*(const Parameter &other) const;
    Parameter operator/(const Parameter &other) const;
    Parameter operator-() const;
};

class ParameterMatrix
{
public:
    ParameterMatrix(const Parameter &parameter);
    explicit ParameterMatrix(const std::vector<std::vector<Parameter>> &matrix);

#if EIGEN_AVAILABLE
    template <typename Derived>
    explicit ParameterMatrix(const Eigen::PlainObjectBase<Derived> &matrix);
    template <typename Derived>
    explicit ParameterMatrix(Eigen::PlainObjectBase<Derived> *matrix);
    template <typename Derived>
    explicit ParameterMatrix(std::function<Eigen::PlainObjectBase<Derived>()> matrix_function);
#endif

    size_t rows() const;
    size_t cols() const;
    std::pair<size_t, size_t> shape() const;
    Parameter operator()(const size_t row, const size_t col) const;
    ParameterMatrix operator+(const ParameterMatrix &other) const;
    ParameterMatrix operator-(const ParameterMatrix &other) const;
    ParameterMatrix operator*(const ParameterMatrix &other) const;
    ParameterMatrix operator*(const Parameter &other) const;
    ParameterMatrix operator/(const Parameter &other) const;
    friend ParameterMatrix operator*(const Parameter &par, const ParameterMatrix &mat);

private:
    std::vector<std::vector<Parameter>> matrix;
};

#ifdef EIGEN_AVAILABLE
template <typename Derived>
ParameterMatrix::ParameterMatrix(const Eigen::PlainObjectBase<Derived> &matrix)
{
    for (size_t row = 0; row < size_t(matrix.rows()); row++)
    {
        std::vector<Parameter> result_row;
        for (size_t col = 0; col < size_t(matrix.cols()); col++)
        {
            result_row.emplace_back(&matrix.coeff(row, col));
        }
        this->matrix.push_back(result_row);
    }
}

template <typename Derived>
ParameterMatrix::ParameterMatrix(Eigen::PlainObjectBase<Derived> *matrix)
{
    for (size_t row = 0; row < size_t(matrix->rows()); row++)
    {
        std::vector<Parameter> result_row;
        for (size_t col = 0; col < size_t(matrix->cols()); col++)
        {
            result_row.emplace_back(&matrix->coeff(row, col));
        }
        this->matrix.push_back(result_row);
    }
}

template <typename Derived>
ParameterMatrix::ParameterMatrix(std::function<Eigen::PlainObjectBase<Derived>()> matrix_function)
{
    auto matrix = matrix_function();
    for (size_t row = 0; row < size_t(matrix->rows()); row++)
    {
        std::vector<Parameter> result_row;
        for (size_t col = 0; col < size_t(matrix->cols()); col++)
        {
            result_row.emplace_back(matrix->coeff(row, col));
        }
        this->matrix.push_back(result_row);
    }
}
#endif

} // namespace op