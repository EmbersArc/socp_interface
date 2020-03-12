#pragma once

#include "wrapperBase.hpp"

#include "EiCOS/include/eicos.hpp"

namespace op
{

class EicosWrapper : public WrapperBase
{
    using WrapperBase::WrapperBase;

    EiCOS::exitcode last_exit_flag;

    std::unique_ptr<EiCOS::Solver> solver;

public:
    void initialize() override;
    bool solveProblem(bool verbose = false) override;
    std::string getResultString() const override;
};

} // namespace op