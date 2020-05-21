#pragma once

#include <functional>
#include <string>
#include <utility>
#include <variant>
#include <memory>

#include <Eigen/Dense>

namespace op
{

    struct Term;
    class Scalar;

    class Parameter
    {
    public:
        Parameter() = default;
        explicit Parameter(const double const_value);
        explicit Parameter(double *value_ptr);

        double get_value() const;

        bool is_constant() const;
        bool is_pointer() const;

        bool is_zero() const;
        bool is_one() const;

        bool operator==(const Parameter &other) const;

        Parameter operator+(const Parameter &other) const;
        Parameter operator-(const Parameter &other) const;
        Parameter operator*(const Parameter &other) const;
        Parameter operator/(const Parameter &other) const;
        operator Term() const;
        operator Scalar() const;

    private:
        enum class Opcode
        {
            Add,
            Sub,
            Mul,
            Div
        };

        Parameter(Opcode op, const Parameter &p1, const Parameter &p2);

        using operation_source_t = std::pair<Opcode, std::vector<Parameter>>;
        using source_variant_t = std::variant<double,
                                              const double *,
                                              operation_source_t>;
        source_variant_t source;
    };

} // namespace op