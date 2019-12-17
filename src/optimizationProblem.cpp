#include "optimizationProblem.hpp"

#include <cassert>
#include <numeric>

namespace op
{

inline size_t tensor_index(const std::vector<size_t> &indices, const std::vector<size_t> &dimensions)
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

void GenericOptimizationProblem::createTensorVariable(const std::string &name, const std::vector<size_t> &dimensions)
{
    size_t tensor_size = std::accumulate(dimensions.begin(), dimensions.end(), 1, std::multiplies<size_t>());

    std::vector<size_t> new_variable_indices(tensor_size);
    std::generate(new_variable_indices.begin(), new_variable_indices.end(), [this]() { return allocateVariableIndex(); });

    tensor_variable_dimensions[name] = dimensions;
    tensor_variable_indices[name] = new_variable_indices;
}

size_t GenericOptimizationProblem::getTensorVariableIndex(const std::string &name, const std::vector<size_t> &indices)
{
    assert(tensor_variable_indices.count(name) > 0);
    std::vector<size_t> &dims = tensor_variable_dimensions[name];
    assert(indices.size() == dims.size());
    for (size_t i = 0; i < indices.size(); i++)
    {
        assert(indices[i] < dims[i]);
    }
    return tensor_variable_indices[name][tensor_index(indices, dims)];
}

Variable GenericOptimizationProblem::getVariable(const std::string &name, const std::vector<size_t> &indices)
{
    Variable var;
    var.name = name;
    var.tensor_indices = indices;
    var.problem_index = getTensorVariableIndex(name, indices);
    return var;
}

} // end namespace op
