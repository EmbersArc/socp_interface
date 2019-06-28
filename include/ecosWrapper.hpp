#pragma once

#include <vector>
#include <map>
#include <string>
#include <cassert>
#include <sstream>
#include <optional>
#include <utility>
#include <functional>

#include "ecos.h"

#include "optimizationProblem.hpp"

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

    /* ECOS result */
    std::vector<double> ecos_solution_vector;
    idxint ecos_exitflag;

  public:
    explicit EcosWrapper(op::SecondOrderConeProgram &_socp);

    void solveProblem(bool verbose = false);

    double getSolutionValue(size_t problem_index) const;
    double getSolutionValue(const std::string &name, const std::vector<size_t> &indices);
    std::vector<double> getSolutionVector() const;
};