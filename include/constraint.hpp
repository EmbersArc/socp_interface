#include "expression.hpp"

namespace op
{

    struct EqualityConstraint
    {
        Affine affine;
    };

    struct PositiveConstraint
    {
        Affine affine;
    };

    struct SecondOrderConeConstraint
    {
        std::vector<Affine> norm;
        Affine affine;
    };

    class Constraint
    {
    public:
        enum class Type
        {
            Equality,
            Positive,
            SecondOrderCone
        };

        Type getType() const;
        void asEquality(const Affine &affine);
        void asPositive(const Affine &affine);
        void asSecondOrderCone(const std::vector<Affine> &norm, const Affine &affine);

    private:
        using constraint_variant_t = std::variant<EqualityConstraint,
                                                  PositiveConstraint,
                                                  SecondOrderConeConstraint>;
        constraint_variant_t data;
    };

} // namespace op