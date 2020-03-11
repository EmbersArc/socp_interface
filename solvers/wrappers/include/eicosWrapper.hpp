#pragma once

#include "secondOrderConeProgram.hpp"

#include "EiCOS/include/eicos.hpp"

#include <vector>
#include <string>

namespace op
{

class EicosWrapper
{
    SecondOrderConeProgram &socp;

    /* ECOS problem parameters */
    int ecos_n_variables;
    int ecos_n_constraint_rows;
    int ecos_n_equalities;
    int ecos_n_positive_constraints;
    int ecos_n_cone_constraints;
    std::vector<int> ecos_cone_constraint_dimensions;
    int ecos_n_exponential_cones;
    std::vector<internal::ParameterSource> ecos_G_data_CCS;
    std::vector<int> ecos_G_columns_CCS;
    std::vector<int> ecos_G_rows_CCS;
    std::vector<internal::ParameterSource> ecos_A_data_CCS;
    std::vector<int> ecos_A_columns_CCS;
    std::vector<int> ecos_A_rows_CCS;
    std::vector<internal::ParameterSource> ecos_cost_function_weights;
    std::vector<internal::ParameterSource> ecos_h;
    std::vector<internal::ParameterSource> ecos_b;
    EiCOS::exitcode last_exit_flag;

    std::unique_ptr<EiCOS::Solver> solver;

    std::vector<double> ecos_cost_function_weights_values;
    std::vector<double> ecos_h_values;
    std::vector<double> ecos_b_values;
    std::vector<double> ecos_G_data_CCS_values;
    std::vector<double> ecos_A_data_CCS_values;

public:
    explicit EicosWrapper(SecondOrderConeProgram &_socp);
    bool solveProblem(bool verbose = false);
    std::string getResultString() const;
};

} // namespace op