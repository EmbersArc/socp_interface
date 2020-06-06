#include "constraint.hpp"

namespace op
{

    std::ostream &operator<<(std::ostream &os, const EqualityConstraint &constraint)
    {
        os << constraint.affine << " == 0";
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const PositiveConstraint &constraint)
    {
        os << "0 <= " << constraint.affine;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const SecondOrderConeConstraint &constraint)
    {
        os << "(";
        for (size_t i = 0; i < constraint.norm.size(); i++)
        {
            os << "(" << constraint.norm[i] << ")^2 ";
            if (i != constraint.norm.size() - 1)
            {
                os << " + ";
            }
        }
        os << ")^(1/2)";

        os << " <= " << constraint.affine;

        return os;
    }

    std::ostream &operator<<(std::ostream &os, const Constraint &constraint)
    {
        switch (constraint.data.index())
        {
        case 0:
            os << std::get<0>(constraint.data);
            break;
        case 1:
            os << std::get<1>(constraint.data);
            break;
        case 2:
            os << std::get<2>(constraint.data);
            break;
        }
        return os;
    }

    Constraint::Type Constraint::getType() const
    {
        return Type(data.index());
    }

    // affine == 0
    void Constraint::asEquality(const Affine &affine)
    {
        EqualityConstraint constraint;
        constraint.affine = affine;
        data = constraint;
    }

    // affine >= 0
    void Constraint::asPositive(const Affine &affine)
    {
        PositiveConstraint constraint;
        constraint.affine = affine;
        data = constraint;
    }

    // norm <= affine
    void Constraint::asSecondOrderCone(const std::vector<Affine> &norm, const Affine &affine)
    {
        SecondOrderConeConstraint constraint;
        constraint.norm = norm;
        constraint.affine = affine;
        data = constraint;
    }

    std::vector<Constraint> equalTo(const Expression &lhs, const Expression &rhs)
    {
        Constraint constraint;
        constraint.asEquality(lhs.affine - rhs.affine);
        return {constraint};
    }

    std::vector<Constraint> lessThan(const Expression &lhs, const Expression &rhs)
    {
        Constraint constraint;
        if (lhs.isNorm())
        {
            std::vector<Affine> norm2_terms;
            for (const std::vector<Affine> &affine : lhs.higher_order)
            {
                norm2_terms.push_back(affine[0]);
            }
            constraint.asSecondOrderCone(norm2_terms, rhs.affine - lhs.affine);
        }
        else if (lhs.getOrder() < 2)
        {
            if (lhs.getOrder() > 0 or rhs.getOrder() > 0)
                constraint.asPositive(rhs.affine - lhs.affine);
        }
        else
        {
            throw std::runtime_error("Found unsupported term in constraint.");
        }
        return {constraint};
    }

    std::vector<Constraint> greaterThan(const Expression &lhs, const Expression &rhs)
    {
        return lessThan(rhs, lhs);
    }

} // namespace op
