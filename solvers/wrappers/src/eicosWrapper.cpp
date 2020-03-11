#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <tuple>
#include <functional>
#include <cassert>
#include <sstream>

#include "eicosWrapper.hpp"

using std::map;
using std::pair;
using std::tuple;
using std::vector;

namespace op
{

bool check_unique_variables_in_affine_expression(const internal::AffineSum &affineSum)
{
    // check if a variable is used more than once in an expression

    vector<size_t> variable_indices;
    for (const auto &term : affineSum.terms)
    {
        if (term.variable)
        { // only consider linear terms, not constant terms
            const size_t idx = term.variable.value().getProblemIndex();
            if (std::find(variable_indices.begin(), variable_indices.end(), idx) != variable_indices.end())
            {
                // duplicate found!
                return false;
            }
            variable_indices.push_back(idx);
        }
    }
    return true;
}

void error_check_affine_expression(const internal::AffineSum &affineSum)
{
    if (!check_unique_variables_in_affine_expression(affineSum))
    {
        std::stringstream ss;
        ss << "Error: Duplicate variable in the expression: \n"
           << affineSum;
        throw std::runtime_error(ss.str());
    }
}

internal::ParameterSource accumulate_constants(const internal::AffineSum &affineSum)
{
    auto pick_constant = [](const internal::ParameterSource &sum, const internal::AffineTerm &term) {
        if (term.variable)
        {
            return sum;
        }
        else
        {
            return sum + term.parameter;
        }
    };

    return std::accumulate(affineSum.terms.begin(),
                           affineSum.terms.end(),
                           internal::ParameterSource(0.),
                           pick_constant);
}

// convert sparse matrix format "dictionary of keys" to "column compressed storage"
void sparse_DOK_to_CCS(
    const map<pair<int, int>, internal::ParameterSource> &sparse_DOK,
    vector<internal::ParameterSource> &data_CCS,
    vector<int> &columns_CCS,
    vector<int> &rows_CCS,
    size_t n_columns)
{
    using std::get;
    assert(data_CCS.empty());
    assert(columns_CCS.empty());
    assert(rows_CCS.empty());

    // convert to coordinate list
    vector<tuple<int, int, internal::ParameterSource>> sparse_COO;
    sparse_COO.reserve(sparse_DOK.size());
    std::transform(sparse_DOK.begin(), sparse_DOK.end(), std::back_inserter(sparse_COO),
                   [](const auto &e) { return std::make_tuple(e.first.first, e.first.second, e.second); });
    //                                                  row index ^    column index ^      value ^

    // sort coordinate list by column, then by row
    std::sort(sparse_COO.begin(), sparse_COO.end(),
              [](const tuple<int, int, internal::ParameterSource> &a,
                 const tuple<int, int, internal::ParameterSource> &b) -> bool {
                  // define coordinate list order
                  if (get<1>(a) == get<1>(b))
                  {
                      return get<0>(a) < get<0>(b);
                  }
                  return get<1>(a) < get<1>(b);
              });

    // create CCS format
    vector<size_t> non_zero_count_per_column(n_columns, 0);
    for (const auto &s : sparse_COO)
    {
        data_CCS.push_back(get<2>(s));
        rows_CCS.push_back(get<0>(s));
        non_zero_count_per_column[get<1>(s)]++;
    }
    columns_CCS.push_back(0);
    for (const auto column_count : non_zero_count_per_column)
    {
        columns_CCS.push_back(column_count + columns_CCS.back());
    }
}

void copy_affine_expression_linear_parts_to_sparse_DOK(
    map<pair<int, int>, internal::ParameterSource> &sparse_DOK,
    const internal::AffineSum &affineSum,
    size_t row_index)
{
    for (const auto &term : affineSum.terms)
    {
        if (term.variable)
        { // only consider linear terms, not constant terms
            size_t column_index = term.variable.value().getProblemIndex();
            sparse_DOK[std::make_pair(row_index, column_index)] = term.parameter;
        }
    }
}

EicosWrapper::EicosWrapper(SecondOrderConeProgram &_socp) : socp(_socp)
{
    socp.cleanUp();

    ecos_cone_constraint_dimensions.clear();
    ecos_G_data_CCS.clear();
    ecos_G_columns_CCS.clear();
    ecos_G_rows_CCS.clear();
    ecos_A_data_CCS.clear();
    ecos_A_columns_CCS.clear();
    ecos_A_rows_CCS.clear();
    ecos_cost_function_weights.clear();
    ecos_h.clear();
    ecos_b.clear();

    /* ECOS size parameters */
    ecos_n_variables = socp.getNumVariables();
    ecos_n_cone_constraints = socp.secondOrderConeConstraints.size();
    ecos_n_equalities = socp.equalityConstraints.size();
    ecos_n_positive_constraints = socp.positiveConstraints.size();
    ecos_n_constraint_rows = socp.positiveConstraints.size();
    ecos_n_exponential_cones = 0; // Exponential cones are not supported.
    for (const auto &cone : socp.secondOrderConeConstraints)
    {
        ecos_n_constraint_rows += 1 + cone.norm2.arguments.size();
        ecos_cone_constraint_dimensions.push_back(1 + cone.norm2.arguments.size());
    }

    /* Error checking for the problem description */
    for (const auto &cone : socp.secondOrderConeConstraints)
    {
        error_check_affine_expression(cone.affine);
        for (const auto &affine_expression : cone.norm2.arguments)
        {
            error_check_affine_expression(affine_expression);
        }
    }
    for (const auto &PositiveConstraint : socp.positiveConstraints)
    {
        error_check_affine_expression(PositiveConstraint.affine);
    }
    for (const auto &equalityConstraint : socp.equalityConstraints)
    {
        error_check_affine_expression(equalityConstraint.affine);
    }
    error_check_affine_expression(socp.costFunction);

    /* Build equality constraint parameters (b - A*x == 0) */
    {
        // Construct the sparse A matrix in the "Dictionary of keys" format
        map<pair<int, int>, internal::ParameterSource> A_sparse_DOK;
        vector<internal::ParameterSource> b(socp.equalityConstraints.size());

        for (size_t i = 0; i < socp.equalityConstraints.size(); i++)
        {
            const auto &affine_expression = socp.equalityConstraints[i].affine;
            b[i] = accumulate_constants(affine_expression);
            copy_affine_expression_linear_parts_to_sparse_DOK(A_sparse_DOK, affine_expression, i);
        }

        // Convert A to "column compressed storage"
        sparse_DOK_to_CCS(A_sparse_DOK, ecos_A_data_CCS, ecos_A_columns_CCS, ecos_A_rows_CCS, ecos_n_variables);
        ecos_b = b;
    }

    /* Build inequality constraint parameters */
    {
        // Construct the sparse G matrix in the "Dictionary of keys" format
        map<pair<int, int>, internal::ParameterSource> G_sparse_DOK;
        vector<internal::ParameterSource> h(ecos_n_constraint_rows);

        size_t row_index = 0;

        for (const auto &PositiveConstraint : socp.positiveConstraints)
        {
            h[row_index] = accumulate_constants(PositiveConstraint.affine);
            copy_affine_expression_linear_parts_to_sparse_DOK(G_sparse_DOK, PositiveConstraint.affine, row_index);
            row_index++;
        }

        for (const auto &secondOrderConeConstraint : socp.secondOrderConeConstraints)
        {
            h[row_index] = accumulate_constants(secondOrderConeConstraint.affine);
            copy_affine_expression_linear_parts_to_sparse_DOK(G_sparse_DOK, secondOrderConeConstraint.affine, row_index);
            row_index++;

            for (const auto &norm2argument : secondOrderConeConstraint.norm2.arguments)
            {
                h[row_index] = accumulate_constants(norm2argument);
                copy_affine_expression_linear_parts_to_sparse_DOK(G_sparse_DOK, norm2argument, row_index);
                row_index++;
            }
        }

        assert(row_index == size_t(ecos_n_constraint_rows)); // all rows used?

        // Convert G to "column compressed storage"
        sparse_DOK_to_CCS(G_sparse_DOK, ecos_G_data_CCS, ecos_G_columns_CCS, ecos_G_rows_CCS, ecos_n_variables);
        ecos_h = h;
    }

    /* Build cost function parameters */
    {
        vector<internal::ParameterSource> c(ecos_n_variables);
        for (const auto &term : socp.costFunction.terms)
        {
            if (term.variable)
            {
                c[term.variable.value().getProblemIndex()] = term.parameter;
            }
        }
        ecos_cost_function_weights = c;
    }

    ecos_G_data_CCS_values.resize(ecos_G_data_CCS.size());
    ecos_A_data_CCS_values.resize(ecos_A_data_CCS.size());
    ecos_cost_function_weights_values.resize(ecos_n_variables);
    ecos_h_values.resize(ecos_n_constraint_rows);
    ecos_b_values.resize(ecos_n_equalities);

    solver = std::make_unique<EiCOS::Solver>(ecos_n_variables,
                                             ecos_n_constraint_rows,
                                             ecos_n_equalities,
                                             ecos_n_positive_constraints,
                                             ecos_n_cone_constraints,
                                             ecos_cone_constraint_dimensions.data(),
                                             ecos_G_data_CCS_values.data(),
                                             ecos_G_columns_CCS.data(),
                                             ecos_G_rows_CCS.data(),
                                             ecos_A_data_CCS_values.data(),
                                             ecos_A_columns_CCS.data(),
                                             ecos_A_rows_CCS.data(),
                                             ecos_cost_function_weights_values.data(),
                                             ecos_h_values.data(),
                                             ecos_b_values.data());
}

void get_parameter_values(const vector<internal::ParameterSource> &params,
                          double factor,
                          std::vector<double> &values)
{
    assert(values.size() == params.size());
    std::transform(params.begin(), params.end(), values.begin(),
                   [factor](const auto &param) { return param.get_value() * factor; });
}

bool EicosWrapper::solveProblem(bool /* verbose */)
{
    get_parameter_values(ecos_cost_function_weights, 1.0, ecos_cost_function_weights_values);
    get_parameter_values(ecos_h, 1.0, ecos_h_values);
    get_parameter_values(ecos_b, 1.0, ecos_b_values);
    get_parameter_values(ecos_G_data_CCS, -1.0, ecos_G_data_CCS_values);
    get_parameter_values(ecos_A_data_CCS, -1.0, ecos_A_data_CCS_values);
    // The signs for A and G must be flipped because they are negative in the ECOS interface

    solver->updateData(ecos_G_data_CCS_values.data(),
                       ecos_A_data_CCS_values.data(),
                       ecos_cost_function_weights_values.data(),
                       ecos_h_values.data(),
                       ecos_b_values.data());

    EiCOS::exitcode ecos_exitflag = solver->solve();

    // copy solution
    for (int i = 0; i < solver->solution().size(); i++)
    {
        socp.solution_vector[i] = solver->solution()(i);
    }

    last_exit_flag = ecos_exitflag;

    return ecos_exitflag != EiCOS::exitcode::fatal;
}

std::string EicosWrapper::getResultString() const
{
    switch (last_exit_flag)
    {
    case EiCOS::exitcode::optimal:
        return "Optimal solution found.";
    case EiCOS::exitcode::primal_infeasible:
        return "Certificate of primal infeasibility found.";
    case EiCOS::exitcode::dual_infeasible:
        return "Certificate of dual infeasibility found.";

    case EiCOS::exitcode::close_to_optimal:
        return "Optimal solution found subject to reduced tolerances.";
    case EiCOS::exitcode::close_to_primal_infeasible:
        return "Certificate of primal infeasibility found subject to reduced tolerances.";
    case EiCOS::exitcode::close_to_dual_infeasible:
        return "Certificate of dual infeasibility found subject to reduced tolerances.";

    case EiCOS::exitcode::maxit:
        return "Maximum number of iterations reached.";

    case EiCOS::exitcode::numerics:
        return "Numerical problems (unreliable search direction).";
    case EiCOS::exitcode::outcone:
        return "Numerical problems (slacks or multipliers outside cone)";

    default: // EiCOS::exitcode::fatal
        return "Unknown problem in solver.";
    }
}

} // namespace op
