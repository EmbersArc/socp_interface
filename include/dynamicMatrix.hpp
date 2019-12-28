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
    T &operator()(size_t row, size_t col = 0);
    void resize(size_t rows, size_t cols);
    bool operator==(const DynamicMatrix &other);

private:
    std::vector<std::vector<T>> matrix;
};

template <typename T>
DynamicMatrix<T>::DynamicMatrix(size_t rows, size_t cols)
{
    resize(rows, cols);
}

template <typename T>
DynamicMatrix<T>::DynamicMatrix(const std::vector<std::vector<T>> &matrix)
    : matrix(matrix) {}

template <typename T>
size_t DynamicMatrix<T>::rows() const
{
    return matrix.size();
}

template <typename T>
size_t DynamicMatrix<T>::cols() const
{
    return matrix.front().size();
}

template <typename T>
T &DynamicMatrix<T>::operator()(size_t row, size_t col)
{
    return matrix[row][col];
}

template <typename T>
void DynamicMatrix<T>::resize(size_t rows, size_t cols)
{
    matrix.resize(rows);
    for (auto &row : matrix)
    {
        row.resize(cols);
    }
}

template <typename T>
bool DynamicMatrix<T>::operator==(const DynamicMatrix<T> &other)
{
    return matrix == other.matrix;
}

} // namespace op