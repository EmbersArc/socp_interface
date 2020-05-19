#include "scalar.hpp"

namespace op
{
    Expression::Expression()
        : parameter(0.) {}

    bool Expression::operator==(const Expression &other) const
    {
        return this->parameter == other.parameter and this->variables == other.variables;
    }

    bool Expression::is_constant() const
    {
        return variables.empty();
    }

    bool Expression::is_linear() const
    {
        return variables.size() == 1;
    }

    bool Expression::is_quadratic() const
    {
        return variables.size() == 2 and variables[0] == variables[1];
    }

    size_t Expression::get_order() const
    {
        return variables.size();
    }

    std::ostream &operator<<(std::ostream &os, const Expression &term)
    {
        os << term.parameter.get_value();
        for (const auto &var : term.variables)
        {
            os << "*" << var;
        }
        return os;
    }

    double Expression::evaluate(const std::vector<double> &soln_values) const
    {
        double product = 1.;
        for (const auto &var : variables)
        {
            product *= soln_values[var.getProblemIndex()];
        }
        return parameter.get_value() * product;
    }

    Expression operator*(const Parameter &parameter, const Variable &variable)
    {
        Expression term;
        term.parameter = parameter;
        term.variables.push_back(variable);

        return term;
    }

    Parameter::operator Expression() const
    {
        Expression term;
        term.parameter = *this;
        return term;
    }

    Variable::operator Expression() const
    {
        Expression term;
        term.parameter = Parameter(1.);
        term.variables.push_back(*this);
        return term;
    }

    Parameter::operator Scalar() const
    {
        Scalar scalar;
        scalar.polyPart = {Expression(*this)};
        return scalar;
    }

    Variable::operator Scalar() const
    {
        Scalar scalar;
        scalar.polyPart = {Expression(*this)};
        return scalar;
    }

    Scalar::Scalar(double x)
    {
        polyPart = {Parameter(x)};
    }

    bool Scalar::operator==(const Scalar &other) const
    {
        return this->polyPart == other.polyPart and this->normPart == other.normPart and this->squared == other.squared;
    }

    std::ostream &operator<<(std::ostream &os, const Scalar &scalar)
    {
        if (not scalar.normPart.empty())
        {
            os << "|| ";
            for (size_t i = 0; i < scalar.normPart.size(); i++)
            {
                os << scalar.normPart[i];

                if (i != scalar.normPart.size() - 1)
                {
                    os << " + ";
                }
            }
            os << " ||";
        }

        if (not scalar.normPart.empty() and not scalar.polyPart.empty())
            os << " + ";

        if (not scalar.polyPart.empty())
        {
            for (size_t i = 0; i < scalar.polyPart.size(); i++)
            {
                os << scalar.polyPart[i];

                if (i != scalar.polyPart.size() - 1)
                {
                    os << " + ";
                }
            }
        }

        return os;
    }

    bool Scalar::norm_valid() const
    {
        return normPart.empty() and
               std::all_of(polyPart.cbegin(), polyPart.cend(),
                           [](const Expression &e) { return e.is_quadratic() or e.is_constant(); });
    }

    Scalar Scalar::operator+(const Scalar &other) const
    {
        if (not this->normPart.empty() and not other.normPart.empty())
        {
            throw std::runtime_error("Invalid addition found.");
        }

        Scalar result = other;
        result.polyPart.insert(result.polyPart.end(),
                               this->polyPart.begin(),
                               this->polyPart.end());
        return result;
    }

    Scalar Scalar::operator-(const Scalar &other) const
    {
        if (not this->normPart.empty() and not other.normPart.empty())
        {
            throw std::runtime_error("Invalid subtraction found.");
        }

        Scalar result = other;
        for (auto &term : result.polyPart)
        {
            term.parameter = Parameter(-1.) * term.parameter;
        }
        result.polyPart.insert(result.polyPart.end(),
                               this->polyPart.begin(),
                               this->polyPart.end());
        return result;
    }

    // TODO
    Scalar Scalar::operator*(const Scalar &other) const
    {
        if (not this->normPart.empty() or
            not other.normPart.empty())
        {
            throw std::runtime_error("Invalid multiplication found.");
        }

        Scalar result;

        // If they are the same, just mark as squared and expect sqrt() later.
        if (this == &other)
        {
            if (std::any_of(this->polyPart.cbegin(), this->polyPart.cend(),
                            [](const Expression &e) { return e.is_quadratic(); }))
            {
                throw std::runtime_error("Invalid square operation found.");
            }
            result = *this;
            result.squared = true;
        }
        else // Multiply out, this is going to be a quadratic term.
        {
            for (auto &t1 : this->polyPart)
            {
                for (auto &t2 : other.polyPart)
                {
                    Expression product;

                    product.parameter = t1.parameter * t2.parameter;

                    product.variables = t1.variables;
                    product.variables.insert(product.variables.begin(),
                                             t2.variables.begin(),
                                             t2.variables.end());

                    result.polyPart.push_back(product);
                }
            }
        }

        return result;
    }

    Scalar Scalar::operator/(const Scalar &other) const
    {
        if (not this->normPart.empty() or
            not other.normPart.empty() or
            other.polyPart.size() != 1 or
            not other.polyPart[0].is_constant())
        {
            throw std::runtime_error("Invalid division found.");
        }

        Scalar result = *this;

        for (auto &term : result.polyPart)
        {
            term.parameter = term.parameter / other.polyPart[0].parameter;
        }

        return result;
    }

    Scalar sqrt(Scalar s)
    {
        if (not s.normPart.empty())
        {
            throw std::runtime_error("Trying to take the norm twice.");
        }

        s.normPart = s.polyPart;
        s.polyPart.clear();
        return s;
    }

    MatrixXe createVariables(const std::string &name, size_t rows, size_t cols)
    {
        MatrixXe variables(rows, cols);

        for (size_t row = 0; row < rows; row++)
        {
            for (size_t col = 0; col < cols; col++)
            {
                variables(row, col) = Variable(name, row, col);
            }
        }
        return variables;
    }

} // namespace op