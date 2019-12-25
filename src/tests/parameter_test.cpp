#include "parameter.hpp"

#include <iostream>

int main()
{
    double scalar = 42.;
    op::Parameter scalar_param_const(42.0);
    op::Parameter scalar_param_ptr(&scalar);

    op::Parameter result_scalar = scalar_param_const * scalar_param_ptr;

    std::cout << result_scalar.getValue() << "\n";
}