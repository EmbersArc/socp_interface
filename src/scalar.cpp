#include "scalar.hpp"

namespace op
{

    std::ostream &operator<<(std::ostream &os, const Term &term)
    {
        os << term.parameter.get_value();
        if (term.variable.has_value())
            os << " * " << term.variable.value();
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const Affine &affine)
    {
        for (size_t i = 0; i < affine.terms.size(); i++)
        {
            os << affine.terms[i];

            if (i != affine.terms.size() - 1)
            {
                os << " + ";
            }
        }

        return os;
    }

    std::ostream &operator<<(std::ostream &os, const Scalar &)
    {
        return os;
    }

    Term::Term()
        : parameter(0.) {}

    bool Term::operator==(const Term &other) const
    {
        return this->parameter == other.parameter and
               this->variable == other.variable;
    }

    double Term::evaluate(const std::vector<double> &soln_values) const
    {
        if (variable.has_value())
        {
            return parameter.get_value() * soln_values[variable.value().getProblemIndex()];
        }
        else
        {
            return parameter.get_value();
        }
    }

    Term operator*(const Parameter &parameter, const Variable &variable)
    {
        Term term;
        term.parameter = parameter;
        term.variable = variable;

        return term;
    }

    Parameter::operator Term() const
    {
        Term term;
        term.parameter = *this;
        return term;
    }

    Variable::operator Term() const
    {
        Term term;
        term.parameter = Parameter(1.);
        term.variable = *this;
        return term;
    }

    Term::operator Affine() const
    {
        Affine affine;
        affine.terms = {*this};
        return affine;
    }

    Parameter::operator Scalar() const
    {
        Scalar scalar;
        scalar.affine = Term(*this);
        return scalar;
    }

    Variable::operator Scalar() const
    {
        Scalar scalar;
        scalar.affine = Term(*this);
        return scalar;
    }

    double Affine::evaluate(const std::vector<double> &soln_values) const
    {
        double sum = 0;
        for (const Term &term : terms)
        {
            sum += term.evaluate(soln_values);
        }
        return sum;
    }

    bool Affine::operator==(const Affine &other) const
    {
        return this->terms == other.terms;
    }

    Affine Affine::operator+(const Affine &other) const
    {
        Affine result = *this;
        result.terms.insert(result.terms.end(),
                            other.terms.cbegin(),
                            other.terms.cend());
        return result;
    }

    Affine Affine::operator-(const Affine &other) const
    {
        Affine result = *this;
        std::transform(other.terms.cbegin(),
                       other.terms.cend(),
                       std::back_inserter(result.terms),
                       [](Term t) {t.parameter = Parameter(-1) * t.parameter; return t; });
        return result;
    }

    Scalar::Scalar(double x)
    {
        affine = Term(Parameter(x));
    }

    bool Scalar::isFirstOrder() const
    {
        return squared_affine.empty() and not is_norm;
    }

    bool Scalar::isSecondOrder() const
    {
        return not squared_affine.empty() and not is_norm;
    }

    bool Scalar::isNorm() const
    {
        return not squared_affine.empty() and is_norm;
    }

    Scalar Scalar::operator+(const Scalar &other) const
    {
        if (this->is_norm and other.is_norm)
        {
            throw std::runtime_error("Adding two norms is not supported.");
        }

        Scalar result = *this;

        result.affine.terms.insert(result.affine.terms.end(),
                                   other.affine.terms.cbegin(),
                                   other.affine.terms.cend());

        result.squared_affine.insert(result.squared_affine.end(),
                                     other.squared_affine.cbegin(),
                                     other.squared_affine.cend());

        return result;
    }

    Scalar Scalar::operator-(const Scalar &other) const
    {
        if (this->is_norm and other.is_norm)
        {
            throw std::runtime_error("Subtracting two norms is not supported.");
        }

        Scalar result = *this;

        std::transform(other.affine.terms.cbegin(),
                       other.affine.terms.cend(),
                       std::back_inserter(result.affine.terms),
                       [](Term t) {t.parameter = Parameter(-1) * t.parameter; return t; });

        return result;
    }

    Scalar Scalar::operator*(const Scalar &other) const
    {
        if (not this->squared_affine.empty() or
            not other.squared_affine.empty())
        {
            throw std::runtime_error("Cannot square terms again.");
        }

        if (not(*this == other))
        {
            throw std::runtime_error("Only squared terms are supported at this point.");
        }

        Scalar result;
        result.squared_affine = {this->affine};
        return result;
    }

    bool Scalar::operator==(const Scalar &other) const
    {
        return this == &other or
               (this->affine == other.affine and
                this->squared_affine == other.squared_affine and
                this->is_norm == other.is_norm);
    }

    Scalar sqrt(Scalar &s)
    {
        if (s.isFirstOrder() or
            s.isNorm())
        {
            throw std::runtime_error("Listen here you little shit"); // TODO
        }

        s.is_norm = true;

        return s;
    }

    MatrixXe create_variables(const std::string &name, size_t rows, size_t cols)
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

    MatrixXe createParameter(double p)
    {
        MatrixXe parameter(1, 1);
        parameter(0, 0) = Parameter(p);
        return parameter;
    }

    MatrixXe createParameter(double *p)
    {
        MatrixXe parameter(1, 1);
        parameter(0, 0) = Parameter(p);
        return parameter;
    }

} // namespace op