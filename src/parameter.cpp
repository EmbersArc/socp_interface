#include "parameter.hpp"

#include <sstream>
#include <cassert>

namespace op
{

    Parameter::Parameter(const double const_value)
        : source(const_value) {}

    Parameter::Parameter(double *value_ptr)
        : source(value_ptr) {}

    Parameter::Parameter(const std::function<double()> &callback)
        : source(callback) {}

    double Parameter::get_value() const
    {
        switch (source.index())
        {
        case 0:
            return std::get<0>(source);
        case 1:
            return *std::get<1>(source);
        default:
            return std::get<2>(source)();
        }
    }

    bool Parameter::is_constant() const
    {
        return source.index() == 0;
    }

    bool Parameter::is_pointer() const
    {
        return source.index() == 1;
    }

    bool Parameter::is_callback() const
    {
        return source.index() == 2;
    }

    bool Parameter::is_zero() const
    {
        return is_constant() and std::abs(get_value()) < 1e-10;
    }

    bool Parameter::is_one() const
    {
        return is_constant() and std::abs(get_value() - 1.) < 1e-10;
    }

    Parameter Parameter::operator+(const Parameter &other) const
    {
        if (other.is_zero())
        {
            return *this;
        }
        if (is_constant() and other.is_constant())
        {
            return Parameter(get_value() + other.get_value());
        }

        return Parameter([p1 = *this,
                          p2 = other]() {
            return p1.get_value() + p2.get_value();
        });
    }

    Parameter Parameter::operator-(const Parameter &other) const
    {
        if (other.is_zero())
        {
            return *this;
        }
        if (is_constant() and other.is_constant())
        {
            return Parameter(get_value() - other.get_value());
        }

        return Parameter([p1 = *this,
                          p2 = other]() {
            return p1.get_value() - p2.get_value();
        });
    }

    Parameter Parameter::operator*(const Parameter &other) const
    {
        if (is_zero() or other.is_zero())
        {
            return Parameter(0.);
        }
        if (is_constant() and other.is_constant())
        {
            return Parameter(get_value() * other.get_value());
        }

        return Parameter([p1 = *this,
                          p2 = other]() {
            return p1.get_value() * p2.get_value();
        });
    }

    Parameter Parameter::operator/(const Parameter &other) const
    {
        assert(not other.is_zero());

        if (other.is_one())
        {
            return *this;
        }
        if (is_constant() and other.is_constant())
        {
            return Parameter(get_value() / other.get_value());
        }

        return Parameter([p1 = *this,
                          p2 = other]() {
            return p1.get_value() / p2.get_value();
        });
    }

} // namespace op