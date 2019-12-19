#include "parameter.hpp"

#include <sstream>
#include <iostream>

namespace op
{

Parameter::Parameter() : source(0), sourceType(ParameterSourceType::Constant) {}

Parameter::Parameter(double const_value)
    : source(const_value), sourceType(ParameterSourceType::Constant) {}

Parameter::Parameter(const double *dynamic_value_ptr)
    : source(dynamic_value_ptr), sourceType(ParameterSourceType::Pointer)
{
    if (dynamic_value_ptr == nullptr)
        throw std::runtime_error("Parameter(nullptr), Null Pointer Error");
}

Parameter::Parameter(std::function<double()> callback)
    : source(callback), sourceType(ParameterSourceType::Callback)
{
    if (!callback)
        throw std::runtime_error("Parameter(callback), Invalid Callback Error");
}

Parameter::~Parameter() {}

double Parameter::get_value() const
{
    double value = 0.;

    switch (sourceType)
    {
    case ParameterSourceType::Constant:
        value = std::get<0>(source);
        break;
    case ParameterSourceType::Pointer:
        value = *std::get<1>(source);
        break;
    case ParameterSourceType::Callback:
        value = std::get<2>(source)();
        break;
    }

    return value;
}

std::ostream &operator<<(std::ostream &os, const Parameter &parameter)
{
    os << "(" << parameter.get_value() << ")";
    return os;
}

Parameter operator+(const Parameter &lhs, const Parameter &rhs)
{
    return Parameter([lhs, rhs]() { return lhs.get_value() + rhs.get_value(); });
}

Parameter operator-(const Parameter &lhs, const Parameter &rhs)
{
    return Parameter([lhs, rhs]() { return lhs.get_value() - rhs.get_value(); });
}

Parameter operator-(const Parameter &par)
{
    return Parameter([par]() { return -par.get_value(); });
}

Parameter operator*(const Parameter &lhs, const Parameter &rhs)
{
    return Parameter([lhs, rhs]() { return lhs.get_value() * rhs.get_value(); });
}

Parameter operator/(const Parameter &lhs, const Parameter &rhs)
{
    return Parameter([lhs, rhs]() { return lhs.get_value() / rhs.get_value(); });
}

} // namespace op