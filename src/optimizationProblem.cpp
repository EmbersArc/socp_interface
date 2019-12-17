#include "optimizationProblem.hpp"

#include <cassert>

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

} // end namespace op
