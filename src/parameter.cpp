#include "parameter.hpp"

#include <sstream>
#include <cassert>
#include <cmath>

namespace cvx
{

    Parameter::Parameter()
        : source(0.) {}

    Parameter::Parameter(int const_value)
        : source(double(const_value)) {}

    Parameter::Parameter(double const_value)
        : source(const_value) {}

    Parameter::Parameter(double *value_ptr)
        : source(value_ptr) {}

    Parameter::Parameter(Opcode op, const Parameter &p1)
        : source(std::make_pair(op, std::vector({p1}))) {}

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
            const std::vector<Parameter> &p = op_params.second;

            switch (op)
            {
            case Opcode::Add:
                return p[0].getValue() + p[1].getValue();
            case Opcode::Sub:
                return p[0].getValue() - p[1].getValue();
            case Opcode::Mul:
                return p[0].getValue() * p[1].getValue();
            case Opcode::Div:
                return p[0].getValue() / p[1].getValue();
            default: // case Opcode::Sqrt
                return std::sqrt(p[0].getValue());
            }
        }
        }
    }

    Parameter::operator double() const
    {
        return this->getValue();
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

    bool Parameter::isOperation() const
    {
        return source.index() == 2;
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

    Parameter &Parameter::operator+=(const Parameter &other)
    {
        *this = *this + other;
        return *this;
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

    Parameter sqrt(Parameter p)
    {
        if (p.isZero())
        {
            return Parameter(0.);
        }
        if (p.isOne())
        {
            return Parameter(1.);
        }
        if (p.isConstant())
        {
            return Parameter(std::sqrt(p.getValue()));
        }

        return Parameter(Parameter::Opcode::Sqrt, p);
    }

} // namespace cvx