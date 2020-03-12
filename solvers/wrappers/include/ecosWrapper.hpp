#pragma once

#include "wrapperBase.hpp"

#include <vector>
#include <string>

namespace op
{

class EcosWrapper : public WrapperBase
{
    using WrapperBase::WrapperBase;

    long last_exit_flag = -99;

    void *work;

    std::vector<long> cone_constraint_dimensions_l;
    std::vector<long> A_columns_CCS_l;
    std::vector<long> G_columns_CCS_l;
    std::vector<long> A_rows_CCS_l;
    std::vector<long> G_rows_CCS_l;

public:
    void initialize() override;
    bool solveProblem(bool verbose = false) override;
    std::string getResultString() const override;
};

} // namespace op