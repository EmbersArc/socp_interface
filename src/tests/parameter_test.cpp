#include "parameter.hpp"

#include <iostream>

int main()
{
    // Scalar
    double scalar = 42.;
    op::Parameter scalar_param_const(42.0);
    op::Parameter scalar_param_ptr(&scalar);

    op::Parameter result_scalar = scalar_param_ptr + scalar_param_ptr + scalar_param_ptr;
    assert(result_scalar.getValue() == 84.);
    result_scalar = scalar_param_const - scalar_param_ptr;
    assert(result_scalar.getValue() == 0.);
    result_scalar = scalar_param_const * scalar_param_ptr;
    assert(result_scalar.getValue() == 1764.);
    result_scalar = scalar_param_const / scalar_param_ptr;
    assert(result_scalar.getValue() == 1.);

    scalar = 1.;

    result_scalar = scalar_param_const + scalar_param_ptr;
    assert(result_scalar.getValue() == 43.);
    result_scalar = scalar_param_const - scalar_param_ptr;
    assert(result_scalar.getValue() == 41);
    result_scalar = scalar_param_const * scalar_param_ptr;
    assert(result_scalar.getValue() == 42.);
    result_scalar = scalar_param_const / scalar_param_ptr;
    assert(result_scalar.getValue() == 42.);

    // Matrix
}