#pragma once

#include <functional>
#include <string>
#include <utility>
#include <variant>
#include <memory>

#include <Eigen/Dense>

namespace op
{

    class Expression;
    class Affine;

    class Parameter
    {
    public:
        Parameter();
        explicit Parameter(const double const_value);
        explicit Parameter(double *value_ptr);

        double getValue() const;

        bool isConstant() const;
        bool isPointer() const;
        bool isOperation() const;

        bool isZero() const;
        bool isOne() const;

        bool operator==(const Parameter &other) const;

        Parameter operator+(const Parameter &other) const;
        Parameter &operator+=(const Parameter &other);
        Parameter operator-(const Parameter &other) const;
        Parameter operator*(const Parameter &other) const;
        Parameter operator/(const Parameter &other) const;
        operator Affine() const;
        operator Expression() const;

        friend Parameter sqrt(Parameter other);
        friend std::ostream &operator<<(std::ostream &os, const Parameter &parameter);

    private:
        enum class Opcode
        {
            Add,
            Sub,
            Mul,
            Div,
            Sqrt,
        };

        Parameter(Opcode op, const Parameter &p1);
        Parameter(Opcode op, const Parameter &p1, const Parameter &p2);

        using operation_source_t = std::pair<Opcode, std::vector<Parameter>>;
        using source_variant_t = std::variant<double,
                                              const double *,
                                              operation_source_t>;
        source_variant_t source;
    };

} // namespace op