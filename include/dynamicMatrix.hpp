#pragma once

#include <vector>
#include <cassert>
#include <memory>

namespace op
{

template <typename T, class Derived = void>
class DynamicMatrix
{

public:
    using return_t = typename std::conditional_t<std::is_same_v<Derived, void>, DynamicMatrix<T>, Derived>;
    DynamicMatrix();
    DynamicMatrix(size_t rows, size_t cols = 1);
    explicit DynamicMatrix(const std::vector<std::vector<T>> &matrix);

    bool empty() const;
    size_t rows() const;
    size_t cols() const;
    size_t size() const;
    bool is_scalar() const;

    std::pair<size_t, size_t> shape() const;

    std::vector<T> rowElements(size_t index) const;
    std::vector<T> colElements(size_t index) const;

    auto row(size_t index) const;
    auto col(size_t index) const;
    auto block(size_t start_row, size_t start_col,
               size_t n_rows, size_t n_cols) const;
    auto transpose() const;

    T &coeffRef(size_t row, size_t col = 0);

    auto operator()(size_t row, size_t col = 0) const;
    const T &coeff(size_t row, size_t col = 0) const;

    void resize(size_t rows, size_t cols);
    bool operator==(const DynamicMatrix &other) const;

    std::vector<std::vector<T>> data_matrix;
};

template <typename T, class Derived>
DynamicMatrix<T, Derived>::DynamicMatrix()
{
    resize(1, 1);
}

template <typename T, class Derived>
DynamicMatrix<T, Derived>::DynamicMatrix(size_t rows, size_t cols)
{
    resize(rows, cols);
}

template <typename T, class Derived>
DynamicMatrix<T, Derived>::DynamicMatrix(const std::vector<std::vector<T>> &matrix)
    : data_matrix(matrix) {}

template <typename T, class Derived>
bool DynamicMatrix<T, Derived>::empty() const
{
    return rows() == 0;
}

template <typename T, class Derived>
size_t DynamicMatrix<T, Derived>::rows() const
{
    return data_matrix.size();
}

template <typename T, class Derived>
size_t DynamicMatrix<T, Derived>::cols() const
{
    return data_matrix.front().size();
}

template <typename T, class Derived>
size_t DynamicMatrix<T, Derived>::size() const
{
    return rows() * cols();
}

template <typename T, class Derived>
bool DynamicMatrix<T, Derived>::is_scalar() const
{
    return rows() == 1 and cols() == 1;
}

template <typename T, class Derived>
std::vector<T> DynamicMatrix<T, Derived>::rowElements(size_t index) const
{
    assert(index < rows());
    return data_matrix.at(index);
}

template <typename T, class Derived>
std::vector<T> DynamicMatrix<T, Derived>::colElements(size_t index) const
{
    assert(index < cols());
    std::vector<T> column;
    for (const auto &row : data_matrix)
    {
        column.push_back(row.at(index));
    }
    return column;
}

template <typename T, class Derived>
std::pair<size_t, size_t> DynamicMatrix<T, Derived>::shape() const
{
    return {rows(), cols()};
}

template <typename T, class Derived>
auto DynamicMatrix<T, Derived>::operator()(size_t row, size_t col) const
{
    assert(row < rows() and col < cols());
    return_t result = *(return_t *)(this);
    result.data_matrix = {{coeff(row, col)}};
    return result;
}

template <typename T, class Derived>
auto DynamicMatrix<T, Derived>::row(size_t index) const
{
    assert(index < rows());
    return_t result = *(return_t *)(this);
    result.data_matrix = {data_matrix[index]};
    return result;
}

template <typename T, class Derived>
auto DynamicMatrix<T, Derived>::col(size_t index) const
{
    assert(index < cols());
    return_t result = *(return_t *)(this);
    result.data_matrix.clear();
    for (const auto &row : data_matrix)
    {
        result.data_matrix.push_back({row[index]});
    }
    return result;
}

template <typename T, class Derived>
auto DynamicMatrix<T, Derived>::block(size_t start_row, size_t start_col,
                                      size_t n_rows, size_t n_cols) const
{
    assert((start_row + n_rows - 1) < rows() and
           (start_col + n_cols - 1) < cols());
    return_t result = *(return_t *)(this);
    result.resize(n_rows, n_cols);
    for (size_t row = 0; row < n_rows; row++)
    {
        for (size_t col = 0; col < n_cols; col++)
        {
            result.coeffRef(row, col) = coeff(start_row + row, start_col + col);
        }
    }
    return result;
}

template <typename T, class Derived>
auto DynamicMatrix<T, Derived>::transpose() const
{
    return_t result = *(return_t *)(this);
    result.resize(cols(), rows());
    for (size_t row = 0; row < rows(); row++)
    {
        for (size_t col = 0; col < cols(); col++)
        {
            result.coeffRef(col, row) = coeff(row, col);
        }
    }
    return result;
}

template <typename T, class Derived>
const T &DynamicMatrix<T, Derived>::coeff(size_t row, size_t col) const
{
    assert(row < rows() and col < cols());
    return data_matrix[row][col];
}

template <typename T, class Derived>
T &DynamicMatrix<T, Derived>::coeffRef(size_t row, size_t col)
{
    assert(row < rows() and col < cols());
    return data_matrix[row][col];
}

template <typename T, class Derived>
void DynamicMatrix<T, Derived>::resize(size_t rows, size_t cols)
{
    data_matrix.resize(rows);
    for (auto &row : data_matrix)
    {
        row.resize(cols);
    }
}

template <typename T, class Derived>
bool DynamicMatrix<T, Derived>::operator==(const DynamicMatrix<T, Derived> &other) const
{
    return data_matrix == other.data_matrix;
}

template <typename T, class Derived>
T vstack(std::initializer_list<T> elements)
{
    T stacked;
    for (const auto &e : elements)
    {
        stacked.data_matrix.insert(stacked.data_matrix.end(),
                                   e.data_matrix.begin(),
                                   e.data_matrix.end());
    }
    return stacked;
}

template <typename T, class Derived>
T hstack(std::initializer_list<T> elements)
{
    T stacked(elements.begin()->rows(), 0);
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