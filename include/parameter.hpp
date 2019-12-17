#pragma once

#include <functional>
#include <string>

namespace op
{

enum class ParameterSource
{
    Constant,
    Pointer,
    Callback
};

// Represents a parameter p_i in the opt-problem that can be changed between problem evaluations.
// The parameter value can either be constant or dynamically accessed through a pointer or callback.
class Parameter
{
    std::function<double()> callback;
    const double *dynamic_value_ptr = nullptr;
    double const_value = 0;
    ParameterSource parameterSource;

public:
    explicit Parameter(std::function<double()> callback) : callback(callback), parameterSource(ParameterSource::Callback)
    {
        if (!callback)
            throw std::runtime_error("Parameter(callback), Invalid Callback Error");
    }

    explicit Parameter(const double *dynamic_value_ptr) : dynamic_value_ptr(dynamic_value_ptr), parameterSource(ParameterSource::Pointer)
    {
        if (dynamic_value_ptr == nullptr)
            throw std::runtime_error("Parameter(NULL), Null Pointer Error");
    }

    explicit Parameter(double const_value) : const_value(const_value), parameterSource(ParameterSource::Constant) {}

    Parameter() : const_value(0), parameterSource(ParameterSource::Constant) {}

    double get_value() const
    {
        if (parameterSource == ParameterSource::Callback)
        {
            return callback();
        }

        if (parameterSource == ParameterSource::Pointer)
        {
            return *dynamic_value_ptr;
        }

        return const_value;
    }
    std::string print() const;
};

} // namespace op