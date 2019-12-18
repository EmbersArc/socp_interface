#pragma once

#include "constraint.hpp"

#include <map>
#include <vector>

namespace op
{

class GenericOptimizationProblem
{
protected:
    size_t n_variables = 0;

    /* Set of named tensor variables in the optimization problem */
    std::map<std::string, std::vector<size_t>> tensor_variable_dimensions;
    std::map<std::string, std::vector<size_t>> tensor_variable_indices;

    size_t allocateVariableIndex();

public:
    void createTensorVariable(const std::string &name, const std::vector<size_t> &dimensions = {});
    size_t getTensorVariableIndex(const std::string &name, const std::vector<size_t> &indices);
    Variable getVariable(const std::string &name, const std::vector<size_t> &indices);
    size_t getNumVariables() const { return n_variables; }
};

} // namespace op