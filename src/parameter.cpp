#include "parameter.hpp"

#include <sstream>
#include <cassert>

namespace op
{

    Parameter::Parameter(const double const_value)
        : source(const_value) {}

    Parameter::Parameter(double *value_ptr)
        : source(value_ptr) {}

    Parameter::Parameter(Opcode op, const Parameter &p1, const Parameter &p2)
        : source(std::make_pair(op, std::vector({p1, p2}))) {}

    bool Parameter::operator==(const Parameter &other) const
    {
        return this->source == other.source;
    }

    double Parameter::get_value() const
    {
        switch (source.index())
        {
        case 0:
            return std::get<0>(source);
        case 1:
            return *std::get<1>(source);
        default: // case 2
        {
            const operation_source_t &op_params = std::get<2>(source);
            const Opcode op = op_params.first;
            const Parameter &p1 = op_params.second[0];
            const Parameter &p2 = op_params.second[1];

            switch (op)
            {
            case Opcode::Add:
                return p1.get_value() + p2.get_value();
            case Opcode::Sub:
                return p1.get_value() - p2.get_value();
            case Opcode::Mul:
                return p1.get_value() * p2.get_value();
            default: // case Opcode::Div
                return p1.get_value() / p2.get_value();
            }
        }
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

        return Parameter(Opcode::Add, *this, other);
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

        return Parameter(Opcode::Sub, *this, other);
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

        return Parameter(Opcode::Mul, *this, other);
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

        return Parameter(Opcode::Div, *this, other);
    }

} // namespace op