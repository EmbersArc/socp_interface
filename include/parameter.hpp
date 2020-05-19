#pragma once

#include <functional>
#include <string>
#include <utility>
#include <variant>
#include <memory>

#include <Eigen/Dense>

namespace op
{

    struct Expression;
    struct Scalar;

    class Parameter
    {
    public:
        Parameter() = default;
        explicit Parameter(const double const_value);
        explicit Parameter(double *value_ptr);
        explicit Parameter(const std::function<double()> &callback);

        double get_value() const;

        bool is_constant() const;
        bool is_pointer() const;
        bool is_callback() const;

        bool is_zero() const;
        bool is_one() const;

        bool operator==(const Parameter &other) const;

        Parameter operator+(const Parameter &other) const;
        Parameter operator-(const Parameter &other) const;
        Parameter operator*(const Parameter &other) const;
        Parameter operator/(const Parameter &other) const;
        operator Expression() const;
        operator Scalar() const;

    private:
        using source_variant_t = std::variant<double,
                                              const double *,
                                              std::function<double()>>;
        source_variant_t source;
    };

} // namespace op