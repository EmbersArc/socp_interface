#pragma once

#include <string>

#include "optimizationProblem.hpp"

class MosekWrapper
{

    op::SecondOrderConeProgram &socp;

    /* result */
    std::vector<double> solution_vector;

public:
    explicit MosekWrapper(op::SecondOrderConeProgram &_socp) : socp(_socp) {}

    void solveProblem(bool verbose = false);

    double getSolutionValue(size_t problem_index) const
    {
        return solution_vector[problem_index];
    }

    double getSolutionValue(const std::string &name, const std::vector<size_t> &indices)
    {
        return solution_vector[socp.getVariable(name, indices).problem_index];
    }

    std::vector<double> getSolutionVector() const
    {
        if (socp.getNumVariables() > 0 && solution_vector.size() == size_t(socp.getNumVariables()))
        {
            return solution_vector;
        }
        else
        {
            throw std::runtime_error("getSolutionVector(): Solution unavailable.");
        }
    }
};