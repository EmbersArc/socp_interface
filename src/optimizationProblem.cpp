#include "optimizationProblem.hpp"

#include <iostream>
#include <cmath>

using std::string;
using std::vector;

namespace op
{

string Variable::print() const
{
    std::ostringstream s;
    s << name;
    if (tensor_indices.size() > 0)
    {
        s << "[";
        for (size_t i = 0; i < tensor_indices.size(); i++)
        {
            if (i)
                s << ",";
            s << tensor_indices[i];
        }
        s << "]";
    }
    s << "@" << problem_index;
    return s.str();
}

string Parameter::print() const
{
    std::ostringstream s;
    s << "(" << get_value() << ")";
    return s.str();
}

Parameter::operator AffineTerm() const
{
    AffineTerm result;
    result.parameter = *this;
    return result;
}

Parameter::operator AffineExpression() const
{
    return (AffineTerm)(*this);
}

AffineTerm::operator AffineExpression()
{
    AffineExpression result;
    result.terms.push_back(*this);
    return result;
}

string AffineTerm::print() const
{
    std::ostringstream s;
    s << parameter.print();
    if (variable)
        s << "*" << variable.value().print();
    return s.str();
}

string AffineExpression::print() const
{
    std::ostringstream s;
    for (size_t i = 0; i < terms.size(); i++)
    {
        if (i)
            s << "  + ";
        s << terms[i].print();
    }
    return s.str();
}

AffineTerm operator*(const Parameter &parameter, const Variable &variable)
{
    AffineTerm affineTerm;
    affineTerm.parameter = parameter;
    affineTerm.variable = variable;
    return affineTerm;
}

AffineTerm operator*(const Variable &variable, const Parameter &parameter)
{
    return parameter * variable;
}

AffineTerm operator*(const double &const_parameter, const Variable &variable)
{
    AffineTerm affineTerm;
    affineTerm.parameter = Parameter(const_parameter);
    affineTerm.variable = variable;
    return affineTerm;
}

AffineTerm operator*(const Variable &variable, const double &const_parameter)
{
    return const_parameter * variable;
}

AffineExpression operator+(const AffineExpression &lhs, const AffineExpression &rhs)
{
    AffineExpression result;
    result.terms.insert(result.terms.end(), lhs.terms.begin(), lhs.terms.end());
    result.terms.insert(result.terms.end(), rhs.terms.begin(), rhs.terms.end());
    return result;
}

AffineExpression operator+(const AffineExpression &lhs, const double &rhs)
{
    AffineExpression result;
    result.terms.insert(result.terms.end(), lhs.terms.begin(), lhs.terms.end());
    result.terms.push_back(Parameter(rhs));
    return result;
}

AffineExpression operator+(const double &lhs, const AffineExpression &rhs)
{
    return rhs + lhs;
}

string Norm2::print() const
{
    std::ostringstream s;
    s << "norm2([ ";
    for (size_t i = 0; i < arguments.size(); i++)
    {
        if (i)
            s << ", ";
        s << arguments[i].print();
    }
    s << " ])";
    return s.str();
}

string PostiveConstraint::print() const
{
    std::ostringstream s;
    s << lhs.print() << " >= 0";
    return s.str();
}

string EqualityConstraint::print() const
{
    std::ostringstream s;
    s << lhs.print() << " == 0";
    return s.str();
}

string SecondOrderConeConstraint::print() const
{
    std::ostringstream s;
    s << lhs.print() << " <= " << rhs.print();
    return s.str();
}

Norm2 norm2(const vector<AffineExpression> &affineExpressions)
{
    Norm2 n;
    n.arguments = affineExpressions;
    return n;
}

SecondOrderConeConstraint operator<=(const Norm2 &lhs, const AffineExpression &rhs)
{
    SecondOrderConeConstraint socc;
    socc.lhs = lhs;
    socc.rhs = rhs;
    return socc;
}

SecondOrderConeConstraint operator>=(const AffineExpression &lhs, const Norm2 &rhs)
{
    return rhs <= lhs;
}

SecondOrderConeConstraint operator<=(const Norm2 &lhs, const double &rhs)
{
    SecondOrderConeConstraint socc;
    socc.lhs = lhs;
    socc.rhs = Parameter(rhs);
    return socc;
}

SecondOrderConeConstraint operator>=(const double &lhs, const Norm2 &rhs)
{
    return rhs <= lhs;
}

PostiveConstraint operator>=(const AffineExpression &lhs, const double &zero)
{
    assert(zero == 0.0);
    PostiveConstraint result;
    result.lhs = lhs;
    return result;
}

PostiveConstraint operator<=(const double &zero, const AffineExpression &rhs)
{
    return rhs >= zero;
}

EqualityConstraint operator==(const AffineExpression &lhs, const double &zero)
{
    assert(zero == 0.0);
    EqualityConstraint result;
    result.lhs = lhs;
    return result;
}

EqualityConstraint operator==(const double &zero, const AffineExpression &rhs)
{
    return rhs == zero;
}

inline size_t tensor_index(const vector<size_t> &indices, const vector<size_t> &dimensions)
{
    assert(indices.size() == dimensions.size());
    size_t index = 0;
    for (size_t d = 0; d < indices.size(); ++d)
        index = index * dimensions[d] + indices[d];
    return index;
}

size_t GenericOptimizationProblem::allocateVariableIndex()
{
    size_t i = n_variables;
    n_variables++;
    return i;
}

void GenericOptimizationProblem::createTensorVariable(const string &name, const vector<size_t> &dimensions)
{
    size_t tensor_size = std::accumulate(dimensions.begin(), dimensions.end(), 1, std::multiplies<size_t>());

    vector<size_t> new_variable_indices(tensor_size);
    std::generate(new_variable_indices.begin(), new_variable_indices.end(), [this]() { return allocateVariableIndex(); });

    tensor_variable_dimensions[name] = dimensions;
    tensor_variable_indices[name] = new_variable_indices;
}

size_t GenericOptimizationProblem::getTensorVariableIndex(const string &name, const vector<size_t> &indices)
{
    assert(tensor_variable_indices.count(name) > 0);
    vector<size_t> &dims = tensor_variable_dimensions[name];
    assert(indices.size() == dims.size());
    for (size_t i = 0; i < indices.size(); i++)
    {
        assert(indices[i] < dims[i]);
    }
    return tensor_variable_indices[name][tensor_index(indices, dims)];
}

Variable GenericOptimizationProblem::getVariable(const string &name, const vector<size_t> &indices)
{
    Variable var;
    var.name = name;
    var.tensor_indices = indices;
    var.problem_index = getTensorVariableIndex(name, indices);
    return var;
}

void SecondOrderConeProgram::addConstraint(SecondOrderConeConstraint c)
{
    secondOrderConeConstraints.push_back(c);
}

void SecondOrderConeProgram::addConstraint(PostiveConstraint c)
{
    postiveConstraints.push_back(c);
}

void SecondOrderConeProgram::addConstraint(EqualityConstraint c)
{
    equalityConstraints.push_back(c);
}

void SecondOrderConeProgram::addMinimizationTerm(const AffineExpression &c)
{
    costFunction = costFunction + c;
}

void SecondOrderConeProgram::printProblem(std::ostream &out) const
{
    using std::endl;
    out << "Minimize" << endl;
    out << costFunction.print() << endl;

    out << endl
        << "Subject to equality constraints" << endl;
    for (const auto &equalityConstraint : equalityConstraints)
    {
        out << equalityConstraint.print() << endl;
    }

    out << endl
        << "Subject to linear inequalities" << endl;
    for (const auto &postiveConstraint : postiveConstraints)
    {
        out << postiveConstraint.print() << endl;
    }

    out << endl
        << "Subject to cone constraints" << endl;
    for (const auto &secondOrderConeConstraint : secondOrderConeConstraints)
    {
        out << secondOrderConeConstraint.print() << endl;
    }
}

template <typename T>
bool feasibility_check_message(double tol, double val, const T &constraint)
{
    if (val > tol)
    {
        std::cout << "Infeasible solution, constraint value: " + std::to_string(val) << "\n"
                  << constraint.print() << "\n";

        return false;
    }
    return true;
}

bool SecondOrderConeProgram::feasibilityCheck(const vector<double> &soln_values) const
{
    const double tol = 0.1;
    bool feasible = true;
    auto check_feasibility = [&](const auto &constraint) { return feasibility_check_message(tol, constraint.evaluate(soln_values), constraint); };
    auto check_feasibility_abs = [&](const auto &constraint) { return feasibility_check_message(tol, fabs(constraint.evaluate(soln_values)), constraint); };

    feasible &= std::all_of(secondOrderConeConstraints.begin(), secondOrderConeConstraints.end(), check_feasibility);
    feasible &= std::all_of(postiveConstraints.begin(), postiveConstraints.end(), check_feasibility);
    feasible &= std::all_of(equalityConstraints.begin(), equalityConstraints.end(), check_feasibility_abs);

    return feasible;
}

double AffineTerm::evaluate(const vector<double> &soln_values) const
{
    double p = parameter.get_value();
    if (variable)
    {
        return p * soln_values[variable.value().problem_index];
    }
    else
    {
        return p;
    }
}

double AffineExpression::evaluate(const vector<double> &soln_values) const
{
    auto sum_terms = [&soln_values](double sum, const auto &term) { return sum + term.evaluate(soln_values); };
    return std::accumulate(terms.begin(), terms.end(), 0., sum_terms);
}

double Norm2::evaluate(const vector<double> &soln_values) const
{
    auto sum_squares = [&soln_values](double sum, const auto &arg) {
        double val = arg.evaluate(soln_values);
        return val * val;
    };
    return std::sqrt(std::accumulate(arguments.begin(), arguments.end(), 0., sum_squares));
}

double SecondOrderConeConstraint::evaluate(const vector<double> &soln_values) const
{
    return (lhs.evaluate(soln_values) - rhs.evaluate(soln_values));
}

double PostiveConstraint::evaluate(const vector<double> &soln_values) const
{
    return -lhs.evaluate(soln_values);
}

double EqualityConstraint::evaluate(const vector<double> &soln_values) const
{
    return -lhs.evaluate(soln_values);
}

} // end namespace op
