#include "parameter.hpp"

#include <sstream>
#include <cassert>

namespace op
{

    Parameter::Parameter()
        : source(0.) {}

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

    double Parameter::getValue() const
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
                return p1.getValue() + p2.getValue();
            case Opcode::Sub:
                return p1.getValue() - p2.getValue();
            case Opcode::Mul:
                return p1.getValue() * p2.getValue();
            default: // case Opcode::Div
                return p1.getValue() / p2.getValue();
            }
        }
        }
    }

    std::ostream &operator<<(std::ostream &os, const Parameter &parameter)
    {
        os << parameter.getValue();

        return os;
    }

    bool Parameter::isConstant() const
    {
        return source.index() == 0;
    }

    bool Parameter::isPointer() const
    {
        return source.index() == 1;
    }

    bool Parameter::isZero() const
    {
        return isConstant() and std::abs(getValue()) < 1e-10;
    }

    bool Parameter::isOne() const
    {
        return isConstant() and std::abs(getValue() - 1.) < 1e-10;
    }

    Parameter Parameter::operator+(const Parameter &other) const
    {
        if (other.isZero())
        {
            return *this;
        }
        if (isConstant() and other.isConstant())
        {
            return Parameter(getValue() + other.getValue());
        }

        return Parameter(Opcode::Add, *this, other);
    }

    Parameter Parameter::operator-(const Parameter &other) const
    {
        if (other.isZero())
        {
            return *this;
        }
        if (isConstant() and other.isConstant())
        {
            return Parameter(getValue() - other.getValue());
        }

        return Parameter(Opcode::Sub, *this, other);
    }

    Parameter Parameter::operator*(const Parameter &other) const
    {
        if (isZero() or other.isZero())
        {
            return Parameter(0.);
        }
        if (isConstant() and other.isConstant())
        {
            return Parameter(getValue() * other.getValue());
        }

        return Parameter(Opcode::Mul, *this, other);
    }

    Parameter Parameter::operator/(const Parameter &other) const
    {
        assert(not other.isZero());

        if (other.isOne())
        {
            return *this;
        }
        if (isConstant() and other.isConstant())
        {
            return Parameter(getValue() / other.getValue());
        }

        return Parameter(Opcode::Div, *this, other);
    }

} // namespace op