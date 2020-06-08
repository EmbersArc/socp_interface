#include "expressions.hpp"
#include <iostream>

namespace Eigen::internal
{
    bool operator==(const op::Scalar &lhs, const op::Scalar &rhs)
    {
        bool equal = true;

        equal &= lhs.affine == rhs.affine;
        equal &= lhs.higher_order == rhs.higher_order;
        equal &= lhs.sqrt == rhs.sqrt;

        return equal;
    }

} // namespace Eigen::internal

namespace op
{

    std::ostream &operator<<(std::ostream &os, const Term &term)
    {
        if (not term.parameter.isOne())
        {
            os << term.parameter.getValue() << " * ";
        }
        os << term.variable;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const Affine &affine)
    {
        for (size_t i = 0; i < affine.terms.size(); i++)
        {
            os << affine.terms[i];
            if (i != affine.terms.size() - 1)
                os << " + ";
        }

        if (not affine.terms.empty() and not affine.constant.isZero())
        {
            os << " + ";
        }

        if (affine.terms.empty() or not affine.constant.isZero())
        {
            os << affine.constant;
        }

        return os;
    }

    std::ostream &operator<<(std::ostream &os, const Scalar &exp)
    {
        os << exp.affine;

        if (exp.getOrder() == 2)
        {
            os << " + ";
            os << "(";

            for (size_t i = 0; i < exp.higher_order.size(); i++)
            {
                const std::vector<Affine> &hot = exp.higher_order[i];

                if (hot.size() == 1)
                {
                    os << "(" << hot[0] << ")^2";
                }
                else if (hot.size() == 2)
                {
                    os << "(" << hot[0] << ")*(" << hot[1] << ")";
                }

                if (i < exp.higher_order.size() - 1)
                {
                    os << " + ";
                }
            }

            os << ")";

            if (exp.isNorm())
            {
                os << "^(1/2)";
            }
        }
        return os;
    }

    // Term

    Term::Term()
        : parameter(0.) {}

    bool Term::operator==(const Term &other) const
    {
        return this->parameter == other.parameter and
               this->variable == other.variable;
    }

    double Term::evaluate() const
    {
        return parameter.getValue() * variable.getSolution();
    }

    Term &Term::operator*=(const Parameter &param)
    {
        if (param.isOne())
        {
            return *this;
        }
        else
        {
            this->parameter = param * this->parameter;
            return *this;
        }
    }

    Term operator*(const Parameter &parameter, const Variable &variable)
    {
        Term term;

        term.parameter = parameter;
        term.variable = variable;

        return term;
    }

    Variable::operator Term() const
    {
        return Parameter(1.) * *this;
    }

    // Affine

    double Affine::evaluate() const
    {
        double sum = this->constant.getValue();
        for (const Term &term : terms)
        {
            sum += term.evaluate();
        }
        return sum;
    }

    bool Affine::operator==(const Affine &other) const
    {
        return this->terms == other.terms;
    }

    Affine &Affine::operator+=(const Affine &other)
    {
        for (const Term &term : other.terms)
        {
            auto existing_term = std::find_if(this->terms.begin(),
                                              this->terms.end(),
                                              [&term](const Term &t) { return t.variable == term.variable; });
            if (existing_term != this->terms.cend())
            {
                // Variable already exists as term. Add to parameter.
                existing_term->parameter += term.parameter;
            }
            else
            {
                this->terms.push_back(term);
            }
        }

        this->constant += other.constant;

        return *this;
    }

    Affine Affine::operator+(const Affine &other) const
    {
        Affine result = *this;

        result += other;

        return result;
    }

    Affine Affine::operator-(const Affine &other) const
    {
        Affine result = *this;
        result += Affine(Parameter(-1.)) * other;
        return result;
    }

    Affine Affine::operator*(const Affine &other) const
    {
        if (this->isFirstOrder() and other.isFirstOrder())
        {
            throw(std::runtime_error("Invalid multiplication."));
        }
        if (this->isConstant() and other.isConstant())
        {
            return Affine(this->constant * other.constant);
        }
        else
        {
            const Parameter &param = other.isConstant() ? other.constant : this->constant;
            const Affine &affine = this->isFirstOrder() ? *this : other;

            Affine result;

            if (param.isZero())
            {
                return result;
            }
            else if (param.isOne())
            {
                return affine;
            }

            for (Term term : affine.terms)
            {
                if (not term.parameter.isZero())
                {
                    term *= param;
                    result.terms.push_back(term);
                }
            }
            result.constant = param * affine.constant;

            return result;
        }
    }

    bool Affine::isZero() const
    {
        return this->terms.empty() and this->constant.isZero();
    }

    bool Affine::isConstant() const
    {
        return this->terms.empty();
    }

    bool Affine::isFirstOrder() const
    {
        return not this->terms.empty();
    }

    Term::operator Affine() const
    {
        Affine affine;
        affine.terms = {*this};
        return affine;
    }

    Parameter::operator Affine() const
    {
        Affine affine;
        affine.constant = *this;
        return affine;
    }

    // Scalar

    Scalar::Scalar(double x)
    {
        this->affine.constant = Parameter(x);
    }

    double Scalar::evaluate() const
    {
        double sum = 0.;

        for (const std::vector<Affine> &affine_v : this->higher_order)
        {
            if (affine_v.size() == 1)
            {
                sum += std::pow(affine_v[0].evaluate(), 2);
            }
            else if (affine_v.size() == 2)
            {
                sum += affine_v[0].evaluate() * affine_v[1].evaluate();
            }
        }

        if (this->sqrt)
        {
            sum = std::sqrt(sum);
        }

        sum += this->affine.evaluate();

        return sum;
    }

    Scalar::operator double() const
    {
        return this->evaluate();
    }

    Scalar &Scalar::operator+=(const Scalar &other)
    {
        if ((this->isNorm() and other.getOrder() == 2) or
            (other.getOrder() == 2 and other.isNorm()) or
            (this->isNorm() and other.isNorm()))
        {
            throw std::runtime_error("Incompatible addition.");
        }

        this->affine += other.affine;

        this->higher_order.insert(this->higher_order.end(),
                                  other.higher_order.cbegin(),
                                  other.higher_order.cend());

        return *this;
    }

    Scalar Scalar::operator+(const Scalar &other) const
    {
        Scalar result = *this;

        result += other;

        return result;
    }

    Scalar &Scalar::operator-=(const Scalar &other)
    {
        if (other.getOrder() > 1)
        {
            throw std::runtime_error("Subtraction is not supported for higher-order terms.");
        }

        this->affine += Affine(Parameter(-1.)) * other.affine;

        return *this;
    }

    Scalar Scalar::operator-(const Scalar &other) const
    {
        Scalar result = *this;

        result -= other;

        return result;
    }

    Scalar Scalar::operator*(const Scalar &other) const
    {
        if (this->isNorm() or this->getOrder() == 2 or
            other.isNorm() or other.getOrder() == 2)
        {
            throw std::runtime_error("Listen here you little shit.");
        }

        Scalar result;

        if (this->affine.isFirstOrder() and other.affine.isFirstOrder())
        {
            result.higher_order.emplace_back();

            // If quadratic, add only one factor.
            result.higher_order.back().emplace_back(this->affine);

            // If not quadratic, add both factors.
            if (not(other.affine == this->affine))
            {
                result.higher_order.back().emplace_back(other.affine);
            }
        }
        else
        {
            result.affine = this->affine * other.affine;
        }

        return result;
    }

    size_t Scalar::getOrder() const
    {
        if (not this->higher_order.empty())
        {
            return 2;
        }
        else if (this->affine.isFirstOrder())
        {
            return 1;
        }
        else // if (not this->affine.isConstant())
        {
            return 0;
        }
    }

    bool Scalar::isNorm() const
    {
        return this->sqrt;
    }

    Scalar sqrt(const Scalar &scalar)
    {
        Scalar e = scalar;
        const bool all_quadratic = std::all_of(e.higher_order.cbegin(),
                                               e.higher_order.cend(),
                                               [](const std::vector<Affine> &p) { return p.size() == 1; });

        if (all_quadratic and e.affine.isConstant())
        {
            e.sqrt = true;
        }
        else
        {
            throw std::runtime_error("Can only take the square root when all terms are quadratic and no linear terms are present.");
        }

        return e;
    }

    Parameter::operator Scalar() const
    {
        Scalar scalar;
        scalar.affine.constant = *this;
        return scalar;
    }

    Variable::operator Scalar() const
    {
        Scalar scalar;
        scalar.affine.terms = {*this};
        return scalar;
    }

    // Parameter and Variable creation

    VectorX createVariables(const std::string &name, size_t rows)
    {
        VectorX variables(rows);

        for (size_t row = 0; row < rows; row++)
        {
            variables(row) = Variable(name, row, 1);
        }
        return variables;
    }

    MatrixX createVariables(const std::string &name, size_t rows, size_t cols)
    {
        MatrixX variables(rows, cols);

        for (size_t row = 0; row < rows; row++)
        {
            for (size_t col = 0; col < cols; col++)
            {
                variables(row, col) = Variable(name, row, col);
            }
        }
        return variables;
    }

    Scalar createParameter(double m)
    {
        return Parameter(m);
    }

    Scalar createParameter(double *m)
    {
        return Parameter(m);
    }

} // namespace op