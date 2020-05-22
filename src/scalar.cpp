#include "scalar.hpp"

namespace op
{

    std::ostream &operator<<(std::ostream &os, const Term &term)
    {
        os << term.parameter.getValue();

        if (term.variable.has_value())
        {
            os << " * " << term.variable.value();
        }
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

    std::ostream &operator<<(std::ostream &os, const Scalar &scalar)
    {
        if (not scalar.squared_affine.empty())
        {
            os << "(";
            for (size_t i = 0; i < scalar.squared_affine.size(); i++)
            {
                os << "(" << scalar.squared_affine[i] << ")^2 ";
                if (i != scalar.squared_affine.size() - 1)
                {
                    os << " + ";
                }
            }
            os << ")";
            if (scalar.sqrt)
            {
                os << "^(1/2)";
            }
        }

        if (not scalar.affine.terms.empty())
        {
            os << " + " << scalar.affine;
        }

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
            return parameter.getValue() * soln_values[variable.value().getProblemIndex()];
        }
        else
        {
            return parameter.getValue();
        }
    }

    Term &Term::operator*=(const Parameter &param)
    {
        Term term;
        if (param.isZero())
        {
            *this = term;
        }
        else
        {
            this->parameter = param * this->parameter;
        }
        return *this;
    }

    Term operator*(const Parameter &parameter, const Variable &variable)
    {
        Term term;

        if (parameter.isZero())
        {
            term = Parameter(0.);
        }
        else
        {
            term.parameter = parameter;
            term.variable = variable;
        }

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
        return Parameter(1.) * *this;
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

    Affine &Affine::operator+=(const Affine &other)
    {
        *this = *this + other;
        return *this;
    }

    Affine Affine::operator+(const Affine &other) const
    {
        Affine result;

        for (const Term &term : this->terms)
        {
            if (not term.parameter.isZero())
                result.terms.push_back(term);
        }

        for (const Term &term : other.terms)
        {
            if (not term.parameter.isZero())
                result.terms.push_back(term);
        }

        return result;
    }

    Affine Affine::operator-(const Affine &other) const
    {
        Affine result = *this;
        result = result + Affine(Term(Parameter(-1.))) * other;
        return result;
    }

    Affine Affine::operator*(const Affine &other) const
    {
        if (not(this->isConstant() or other.isConstant()))
        {
            throw(std::runtime_error("Invalid multiplication."));
        }

        const Affine &affine = this->isFirstOrder() ? *this : other;
        const Parameter &param = other.isConstant() ? other.terms[0].parameter : this->terms[0].parameter;

        Affine result;
        for (Term term : affine.terms)
        {
            if (not term.parameter.isZero())
            {
                term *= param;
                result.terms.push_back(term);
            }
        }
        return result;
    }

    bool Affine::isConstant() const
    {
        return not isFirstOrder();
    }

    bool Affine::isFirstOrder() const
    {
        return std::any_of(terms.cbegin(),
                           terms.cend(),
                           [](const Term &t) { return t.variable.has_value(); });
    }

    Scalar::Scalar(double x)
    {
        affine = Term(Parameter(x));
    }

    bool Scalar::isConstant() const
    {
        return squared_affine.empty() and affine.isConstant();
    }

    bool Scalar::isFirstOrder() const
    {
        return squared_affine.empty() and not sqrt;
    }

    bool Scalar::isSecondOrder() const
    {
        return not squared_affine.empty() and not sqrt;
    }

    bool Scalar::isNorm() const
    {
        return not squared_affine.empty() and sqrt;
    }

    Scalar &Scalar::operator+=(const Scalar &other)
    {
        if (this->sqrt and other.sqrt)
        {
            throw std::runtime_error("Adding two norms is not supported.");
        }

        this->affine += other.affine;

        this->squared_affine.insert(this->squared_affine.end(),
                                    other.squared_affine.cbegin(),
                                    other.squared_affine.cend());

        return *this;
    }

    Scalar Scalar::operator+(const Scalar &other) const
    {
        Scalar result = *this;

        result += other;

        return result;
    }

    Scalar Scalar::operator-(const Scalar &other) const
    {
        if (this->sqrt and other.sqrt)
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
        if (this->isSecondOrder() or
            other.isSecondOrder())
        {
            throw std::runtime_error("Cannot multiply a sum of squares.");
        }
        else if (this->isNorm() or
                 other.isNorm())
        {
            throw std::runtime_error("Cannot multiply a norm.");
        }

        Scalar result;

        if (this->isConstant() or other.isConstant())
        {
            result.affine = this->affine * other.affine;
        }
        else if (*this == other)
        {
            result.squared_affine = {this->affine};
        }
        else
        {
            throw std::runtime_error("Only squared terms are supported at this point.");
        }

        return result;
    }

    bool Scalar::operator==(const Scalar &other) const
    {
        return this == &other or
               (this->affine == other.affine and
                this->squared_affine == other.squared_affine and
                this->sqrt == other.sqrt);
    }

    Scalar sqrt(Scalar s)
    {
        if (s.isFirstOrder() or
            s.isNorm())
        {
            throw std::runtime_error("listen here you little shit"); // TODO
        }

        s.sqrt = true;

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