#pragma once

#include "secondOrderConeProgram.hpp"

#define DCTRLC = 1
#define DLONG
#define LDL_LONG
#include "ecos.h"

#include <vector>
#include <map>
#include <string>

namespace op
{

class EcosWrapper
{
    SecondOrderConeProgram &socp;

    /* ECOS problem parameters */
    idxint ecos_n_variables;
    idxint ecos_n_constraint_rows;
    idxint ecos_n_equalities;
    idxint ecos_n_positive_constraints;
    idxint ecos_n_cone_constraints;
    std::vector<idxint> ecos_cone_constraint_dimensions;
    idxint ecos_n_exponential_cones;
    std::vector<internal::ParameterSource> ecos_G_data_CCS;
    std::vector<idxint> ecos_G_columns_CCS;
    std::vector<idxint> ecos_G_rows_CCS;
    std::vector<internal::ParameterSource> ecos_A_data_CCS;
    std::vector<idxint> ecos_A_columns_CCS;
    std::vector<idxint> ecos_A_rows_CCS;
    std::vector<internal::ParameterSource> ecos_cost_function_weights;
    std::vector<internal::ParameterSource> ecos_h;
    std::vector<internal::ParameterSource> ecos_b;
    idxint last_exit_flag = -99;

public:
    explicit EcosWrapper(SecondOrderConeProgram &_socp);
    bool solveProblem(bool verbose = false);
    std::string getResultString() const;
};

} // namespace op