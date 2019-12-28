#pragma once

#include <vector>

namespace op
{

template <typename T>
class DynamicMatrix
{
public:
    DynamicMatrix() = default;
    DynamicMatrix(size_t rows, size_t cols);
    explicit DynamicMatrix(const std::vector<std::vector<T>> &matrix);
    size_t rows() const;
    size_t cols() const;
    std::pair<size_t, size_t> shape() const;
    T &operator()(size_t row, size_t col = 0);
    T coeff(size_t row, size_t col = 0) const;
    void resize(size_t rows, size_t cols);
    bool operator==(const DynamicMatrix &other);

    std::vector<std::vector<T>> data_matrix;
};

template <typename T>
DynamicMatrix<T>::DynamicMatrix(size_t rows, size_t cols)
{
    resize(rows, cols);
}

template <typename T>
DynamicMatrix<T>::DynamicMatrix(const std::vector<std::vector<T>> &matrix)
    : data_matrix(matrix) {}

template <typename T>
size_t DynamicMatrix<T>::rows() const
{
    return data_matrix.size();
}

template <typename T>
size_t DynamicMatrix<T>::cols() const
{
    return data_matrix.front().size();
}

template <typename T>
std::pair<size_t, size_t> DynamicMatrix<T>::shape() const
{
    return {rows(), cols()};
}

template <typename T>
T &DynamicMatrix<T>::operator()(size_t row, size_t col)
{
    return data_matrix[row][col];
}

template <typename T>
T DynamicMatrix<T>::coeff(size_t row, size_t col) const
{
    return data_matrix[row][col];
}

template <typename T>
void DynamicMatrix<T>::resize(size_t rows, size_t cols)
{
    data_matrix.resize(rows);
    for (auto &row : data_matrix)
    {
        row.resize(cols);
    }
}

template <typename T>
bool DynamicMatrix<T>::operator==(const DynamicMatrix<T> &other)
{
    return data_matrix == other.data_matrix;
}

} // namespace op