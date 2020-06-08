#pragma once

#include "wrapperBase.hpp"

#include "EiCOS/include/eicos.hpp"

namespace cvx
{

class EicosWrapper : public WrapperBase
{
    using WrapperBase::WrapperBase;

    EiCOS::exitcode last_exit_flag;

    std::unique_ptr<EiCOS::Solver> solver;

    Eigen::SparseMatrix<double> A_values;
    Eigen::SparseMatrix<double> G_values;
    Eigen::VectorXd c_values;
    Eigen::VectorXd h_values;
    Eigen::VectorXd b_values;

public:
    void initialize() override;
    bool solveProblem(bool verbose = false) override;
    std::string getResultString() const override;
};

} // namespace cvx