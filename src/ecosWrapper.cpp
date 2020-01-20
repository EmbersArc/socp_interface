#include <stdexcept>
#include <algorithm>
#include <tuple>
#include <utility>
#include <functional>
#include <cassert>
#include <sstream>

#include "ecosWrapper.hpp"

using std::make_pair;
using std::map;
using std::pair;
using std::tuple;
using std::vector;

static_assert(std::is_same_v<idxint, SuiteSparse_long>,
              "Definitions of idxint might not be consistent."
              "Make sure ECOS is compiled with USE_LONG = 1.");

namespace op
{

bool check_unique_variables_in_affine_expression(const AffineSum &AffineSum)
{
    // check if a variable is used more than once in an expression

    vector<size_t> variable_indices;
    for (const auto &term : AffineSum.terms)
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

size_t count_constants_in_affine_expression(const AffineSum &AffineSum)
{
    return std::count_if(AffineSum.terms.begin(), AffineSum.terms.end(), [](const auto &term) { return !term.variable; });
}

void error_check_affine_expression(const AffineSum &AffineSum)
{
    if (!check_unique_variables_in_affine_expression(AffineSum))
    {
        std::stringstream ss;
        ss << "Error: Duplicate variable in the expression: \n"
           << AffineSum;
        throw std::runtime_error(ss.str());
    }
    if (count_constants_in_affine_expression(AffineSum) > 1)
    {
        std::stringstream ss;
        ss << "Error: More than one constant in the expression: \n"
           << AffineSum;
        throw std::runtime_error(ss.str());
    }
}

ParameterSource get_constant_or_zero(const AffineSum &AffineSum)
{
    auto constantIterator = std::find_if(AffineSum.terms.begin(),
                                         AffineSum.terms.end(),
                                         [](const auto &term) { return !term.variable; });
    if (constantIterator != AffineSum.terms.end())
    {
        return constantIterator->parameter;
    }
    else
    {
        return ParameterSource(0.0);
    }
}

// convert sparse matrix format "dictionary of keys" to "column compressed storage"
void sparse_DOK_to_CCS(
    const map<pair<idxint, idxint>, ParameterSource> &sparse_DOK,
    vector<ParameterSource> &data_CCS,
    vector<idxint> &columns_CCS,
    vector<idxint> &rows_CCS,
    size_t n_columns)
{
    using std::get;
    assert(data_CCS.empty());
    assert(columns_CCS.empty());
    assert(rows_CCS.empty());

    // convert to coordinate list
    vector<tuple<idxint, idxint, ParameterSource>> sparse_COO;
    sparse_COO.reserve(sparse_DOK.size());
    std::transform(sparse_DOK.begin(), sparse_DOK.end(), std::back_inserter(sparse_COO),
                   [](const auto &e) { return std::make_tuple(e.first.first, e.first.second, e.second); });
    //                                                  row index ^    column index ^      value ^

    // sort coordinate list by column, then by row
    std::sort(sparse_COO.begin(), sparse_COO.end(),
              [](const tuple<idxint, idxint, ParameterSource> &a,
                 const tuple<idxint, idxint, ParameterSource> &b) -> bool {
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
    map<pair<idxint, idxint>, ParameterSource> &sparse_DOK,
    const AffineSum &AffineSum,
    size_t row_index)
{
    for (const auto &term : AffineSum.terms)
    {
        if (term.variable)
        { // only consider linear terms, not constant terms
            size_t column_index = term.variable.value().getProblemIndex();
            sparse_DOK[make_pair(row_index, column_index)] = term.parameter;
        }
    }
}

EcosWrapper::EcosWrapper(SecondOrderConeProgram &_socp) : socp(_socp)
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
        map<pair<idxint, idxint>, ParameterSource> A_sparse_DOK;
        vector<ParameterSource> b(socp.equalityConstraints.size());

        for (size_t i = 0; i < socp.equalityConstraints.size(); i++)
        {
            const auto &affine_expression = socp.equalityConstraints[i].affine;
            b[i] = get_constant_or_zero(affine_expression);
            copy_affine_expression_linear_parts_to_sparse_DOK(A_sparse_DOK, affine_expression, i);
        }

        // Convert A to "column compressed storage"
        sparse_DOK_to_CCS(A_sparse_DOK, ecos_A_data_CCS, ecos_A_columns_CCS, ecos_A_rows_CCS, ecos_n_variables);
        ecos_b = b;
    }

    /* Build inequality constraint parameters */
    {
        // Construct the sparse G matrix in the "Dictionary of keys" format
        map<pair<idxint, idxint>, ParameterSource> G_sparse_DOK;
        vector<ParameterSource> h(ecos_n_constraint_rows);

        size_t row_index = 0;

        for (const auto &PositiveConstraint : socp.positiveConstraints)
        {
            h[row_index] = get_constant_or_zero(PositiveConstraint.affine);
            copy_affine_expression_linear_parts_to_sparse_DOK(G_sparse_DOK, PositiveConstraint.affine, row_index);
            row_index++;
        }

        for (const auto &secondOrderConeConstraint : socp.secondOrderConeConstraints)
        {
            h[row_index] = get_constant_or_zero(secondOrderConeConstraint.affine);
            copy_affine_expression_linear_parts_to_sparse_DOK(G_sparse_DOK, secondOrderConeConstraint.affine, row_index);
            row_index++;

            for (const auto &norm2argument : secondOrderConeConstraint.norm2.arguments)
            {
                h[row_index] = get_constant_or_zero(norm2argument);
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
        vector<ParameterSource> c(ecos_n_variables);
        for (const auto &term : socp.costFunction.terms)
        {
            if (term.variable)
            {
                c[term.variable.value().getProblemIndex()] = term.parameter;
            }
        }
        ecos_cost_function_weights = c;
    }
}

inline vector<double> get_parameter_values(const vector<ParameterSource> &params, double factor)
{
    vector<double> result;
    result.reserve(params.size());
    std::transform(params.begin(), params.end(), std::back_inserter(result),
                   [factor](const auto &param) { return param.get_value() * factor; });
    return result;
}

bool EcosWrapper::solveProblem(bool verbose)
{
    vector<double> ecos_cost_function_weights_values = get_parameter_values(ecos_cost_function_weights, 1.0);
    vector<double> ecos_h_values = get_parameter_values(ecos_h, 1.0);
    vector<double> ecos_b_values = get_parameter_values(ecos_b, 1.0);
    vector<double> ecos_G_data_CCS_values = get_parameter_values(ecos_G_data_CCS, -1.0);
    vector<double> ecos_A_data_CCS_values = get_parameter_values(ecos_A_data_CCS, -1.0);
    // The signs for A and G must be flipped because they are negative in the ECOS interface

    pwork *mywork = ECOS_setup(
        ecos_n_variables,
        ecos_n_constraint_rows,
        ecos_n_equalities,
        ecos_n_positive_constraints,
        ecos_n_cone_constraints,
        ecos_cone_constraint_dimensions.data(),
        ecos_n_exponential_cones,
        ecos_G_data_CCS_values.data(),
        ecos_G_columns_CCS.data(),
        ecos_G_rows_CCS.data(),
        ecos_A_data_CCS_values.data(),
        ecos_A_columns_CCS.data(),
        ecos_A_rows_CCS.data(),
        ecos_cost_function_weights_values.data(),
        ecos_h_values.data(),
        ecos_b_values.data());

    idxint ecos_exitflag;

    if (mywork != nullptr)
    {
        mywork->stgs->verbose = verbose;

        ecos_exitflag = ECOS_solve(mywork);

        // copy solution
        for (int i = 0; i < ecos_n_variables; i++)
        {
            socp.solution_vector[i] = mywork->x[i];
        }

        ECOS_cleanup(mywork, 0);
    }
    else
    {
        throw std::runtime_error("Could not set up problem.");
    }

    if (ecos_exitflag == ECOS_SIGINT)
    {
        std::terminate();
    }

    last_exit_flag = ecos_exitflag;

    return ecos_exitflag != ECOS_SIGINT and
           ecos_exitflag != ECOS_FATAL and
           ecos_exitflag != ECOS_PINF + ECOS_INACC_OFFSET and
           ecos_exitflag != ECOS_DINF + ECOS_INACC_OFFSET;
}

std::string EcosWrapper::getResultString() const
{
    switch (last_exit_flag)
    {
    case -99:
        return "Problem not solved yet.";
    case ECOS_OPTIMAL:
        return "Optimal solution found.";
    case ECOS_PINF:
        return "Certificate of primal infeasibility found.";
    case ECOS_DINF:
        return "Certificate of dual infeasibility found.";

    case ECOS_OPTIMAL + ECOS_INACC_OFFSET:
        return "Optimal solution found subject to reduced tolerances.";
    case ECOS_PINF + ECOS_INACC_OFFSET:
        return "Certificate of primal infeasibility found subject to reduced tolerances.";
    case ECOS_DINF + ECOS_INACC_OFFSET:
        return "Certificate of dual infeasibility found subject to reduced tolerances.";

    case ECOS_MAXIT:
        return "Maximum number of iterations reached.";

    case ECOS_NUMERICS:
        return "Numerical problems (unreliable search direction).";
    case ECOS_OUTCONE:
        return "Numerical problems (slacks or multipliers outside cone)";

    case ECOS_SIGINT:
        return "Interrupted by signal or CTRL-C.";
    default: // ECOS_FATAL:
        return "Unknown problem in solver.";
    }
}

} // namespace op
