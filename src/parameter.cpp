#include "parameter.hpp"

#include <sstream>

namespace op
{

Parameter::Parameter() : source(0), sourceType(ParameterSourceType::Constant) {}

Parameter::Parameter(const double *dynamic_value_ptr) : source(dynamic_value_ptr), sourceType(ParameterSourceType::Pointer)
{
    if (dynamic_value_ptr == nullptr)
        throw std::runtime_error("Parameter(NULL), Null Pointer Error");
}

Parameter::Parameter(std::function<double()> callback) : source(callback), sourceType(ParameterSourceType::Callback)
{
    if (!callback)
        throw std::runtime_error("Parameter(callback), Invalid Callback Error");
}

Parameter::Parameter(double const_value) : source(const_value), sourceType(ParameterSourceType::Constant) {}

double Parameter::get_value() const
{
    double value = 0.;

    switch (sourceType)
    {
    case ParameterSourceType::Callback:
        value = std::get<std::function<double()>>(source)();
        break;
    case ParameterSourceType::Pointer:
        value = *std::get<const double *>(source);
        break;
    case ParameterSourceType::Constant:
        value = std::get<double>(source);
        break;
    }

    return value;
}

std::ostream& operator<<(std::ostream& os, const Parameter& parameter)
{
    os << "(" << parameter.get_value() << ")";
    return os;
}

} // namespace op