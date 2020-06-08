#pragma once

#include "wrapperBase.hpp"

#include <any>

namespace cvx
{

class EcosWrapper : public WrapperBase
{
    using WrapperBase::WrapperBase;

    long last_exit_flag = -99;

    std::any work;

    size_t step = 0;

    std::vector<long> cone_constraint_dimensions_l;
    std::vector<long> A_columns_CCS_l;
    std::vector<long> G_columns_CCS_l;
    std::vector<long> A_rows_CCS_l;
    std::vector<long> G_rows_CCS_l;

    Eigen::VectorXd c_values1;
    Eigen::VectorXd h_values1;
    Eigen::VectorXd b_values1;
    Eigen::VectorXd G_data_CCS_values1;
    Eigen::VectorXd A_data_CCS_values1;

    Eigen::VectorXd c_values2;
    Eigen::VectorXd h_values2;
    Eigen::VectorXd b_values2;
    Eigen::VectorXd G_data_CCS_values2;
    Eigen::VectorXd A_data_CCS_values2;

public:
    void initialize() override;
    bool solveProblem(bool verbose = false) override;
    std::string getResultString() const override;
};

} // namespace cvx