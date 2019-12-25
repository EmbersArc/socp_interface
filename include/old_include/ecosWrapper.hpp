#pragma once

#include "secondOrderConeProgram.hpp"

#define DCTRLC = 1
#define DLONG
#define LDL_LONG
#include "ecos.h"

#include <vector>
#include <map>
#include <string>

void sparse_DOK_to_CCS(
    const std::map<std::pair<idxint, idxint>, op::Parameter> &sparse_DOK,
    std::vector<op::Parameter> &data_CCS,
    std::vector<idxint> &columns_CCS,
    std::vector<idxint> &rows_CCS,
    size_t n_columns);

class EcosWrapper
{
    op::SecondOrderConeProgram &socp;

    /* ECOS problem parameters */
    idxint ecos_n_variables;
    idxint ecos_n_constraint_rows;
    idxint ecos_n_equalities;
    idxint ecos_n_positive_constraints;
    idxint ecos_n_cone_constraints;
    std::vector<idxint> ecos_cone_constraint_dimensions;
    idxint ecos_n_exponential_cones;
    std::vector<op::Parameter> ecos_G_data_CCS;
    std::vector<idxint> ecos_G_columns_CCS;
    std::vector<idxint> ecos_G_rows_CCS;
    std::vector<op::Parameter> ecos_A_data_CCS;
    std::vector<idxint> ecos_A_columns_CCS;
    std::vector<idxint> ecos_A_rows_CCS;
    std::vector<op::Parameter> ecos_cost_function_weights;
    std::vector<op::Parameter> ecos_h;
    std::vector<op::Parameter> ecos_b;

public:
    explicit EcosWrapper(op::SecondOrderConeProgram &_socp);

    int solveProblem(bool verbose = false);
};