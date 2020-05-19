#include "scalar.hpp"
#include <iostream>

int main()
{
    op::MatrixXe matrix(3, 3);

    matrix.setIdentity();

    std::cout << matrix << "\n";
}