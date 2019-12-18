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

Parameter::Parameter(std::shared_ptr<ParameterOperation> operation)
    : source(operation), sourceType(ParameterSourceType::Operation) {}

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
    case ParameterSourceType::Operation:
        value = std::get<3>(source)->get_value();
        break;
    }

    return value;
}

std::ostream &operator<<(std::ostream &os, const Parameter &parameter)
{
    os << "(" << parameter.get_value() << ")";
    return os;
}

ParameterOperation::ParameterOperation(const Parameter &lhs, const Parameter &rhs,
                                       const std::function<double(double, double)> &operation)
    : terms(lhs, rhs), operation(operation) {}

ParameterOperation::~ParameterOperation() {}

double ParameterOperation::get_value() const
{
    return operation(terms.first.get_value(), terms.second.get_value());
}

Parameter operator+(const Parameter &lhs, const Parameter &rhs)
{
    return Parameter(std::make_shared<ParameterOperation>(lhs, rhs, std::plus<double>()));
}

Parameter operator-(const Parameter &lhs, const Parameter &rhs)
{
    return Parameter(std::make_shared<ParameterOperation>(lhs, rhs, std::minus<double>()));
}

Parameter operator-(const Parameter &par)
{
    return Parameter(std::make_shared<ParameterOperation>(-1.0, par, std::multiplies<double>()));
}

Parameter operator*(const Parameter &lhs, const Parameter &rhs)
{
    return Parameter(std::make_shared<ParameterOperation>(lhs, rhs, std::multiplies<double>()));
}

Parameter operator/(const Parameter &lhs, const Parameter &rhs)
{
    return Parameter(std::make_shared<ParameterOperation>(lhs, rhs, std::divides<double>()));
}

} // namespace op