#include "expression.hpp"

namespace op
{

    struct EqualityConstraint
    {
        Affine affine;
        friend std::ostream &operator<<(std::ostream &os, const EqualityConstraint &constraint);
    };

    struct PositiveConstraint
    {
        Affine affine;
        friend std::ostream &operator<<(std::ostream &os, const PositiveConstraint &constraint);
    };

    struct SecondOrderConeConstraint
    {
        std::vector<Affine> norm;
        Affine affine;
        friend std::ostream &operator<<(std::ostream &os, const SecondOrderConeConstraint &constraint);
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

        friend std::ostream &operator<<(std::ostream &os, const Constraint &constraint);

    private:
        using constraint_variant_t = std::variant<EqualityConstraint,
                                                  PositiveConstraint,
                                                  SecondOrderConeConstraint>;
        constraint_variant_t data;
    };

} // namespace op