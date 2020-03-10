#pragma once

#include "secondOrderConeProgram.hpp"

#include <vector>
#include <string>

namespace op
{

class EcosWrapper
{
    SecondOrderConeProgram &socp;

    /* ECOS problem parameters */
    long ecos_n_variables;
    long ecos_n_constraint_rows;
    long ecos_n_equalities;
    long ecos_n_positive_constraints;
    long ecos_n_cone_constraints;
    std::vector<long> ecos_cone_constraint_dimensions;
    long ecos_n_exponential_cones;
    std::vector<internal::ParameterSource> ecos_G_data_CCS;
    std::vector<long> ecos_G_columns_CCS;
    std::vector<long> ecos_G_rows_CCS;
    std::vector<internal::ParameterSource> ecos_A_data_CCS;
    std::vector<long> ecos_A_columns_CCS;
    std::vector<long> ecos_A_rows_CCS;
    std::vector<internal::ParameterSource> ecos_cost_function_weights;
    std::vector<internal::ParameterSource> ecos_h;
    std::vector<internal::ParameterSource> ecos_b;
    long last_exit_flag = -99;

public:
    explicit EcosWrapper(SecondOrderConeProgram &_socp);
    bool solveProblem(bool verbose = false);
    std::string getResultString() const;
};

} // namespace op