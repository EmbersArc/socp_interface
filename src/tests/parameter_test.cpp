#include "parameter.hpp"

#include <iostream>
#include <cassert>

int main()
{
    // Scalar
    double scalar = 3.;
    op::Parameter scalar_param_const(2.0);
    op::Parameter scalar_param_ptr(&scalar);

    op::Parameter result_scalar = scalar_param_ptr * scalar_param_const * scalar_param_ptr * scalar_param_ptr;
    assert(result_scalar.getValue() == 9.);

    // scalar = 1.;

    // result_scalar = scalar_param_const + scalar_param_ptr;
    // assert(result_scalar.getValue() == 43.);
    // result_scalar = scalar_param_const - scalar_param_ptr;
    // assert(result_scalar.getValue() == 41);
    // result_scalar = scalar_param_const * scalar_param_ptr;
    // assert(result_scalar.getValue() == 42.);
    // result_scalar = scalar_param_const / scalar_param_ptr;
    // assert(result_scalar.getValue() == 42.);

    // Matrix
}