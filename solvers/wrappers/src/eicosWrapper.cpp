#include "eicosWrapper.hpp"

namespace op
{

    void EicosWrapper::initialize()
    {
        solver = std::make_unique<EiCOS::Solver>(G_values,
                                                 A_values,
                                                 c_values,
                                                 h_values,
                                                 b_values,
                                                 soc_dims);
    }

    bool EicosWrapper::solveProblem(bool verbose)
    {
        assert(solver != nullptr && "You must first call initialize()!");

        G_values = G.cast<double>();
        A_values = A.cast<double>();
        c_values = c.cast<double>();
        h_values = h.cast<double>();
        b_values = b.cast<double>();

        // The signs for A and G must be flipped because they are negative in the EiCOS interface
        G_values *= -1;
        A_values *= -1;

        solver->updateData(G_values, A_values, c_values, h_values, b_values);

        EiCOS::exitcode exitflag = solver->solve(verbose);

        // copy solution
        solution = std::vector<double>(solver->solution().data(),
                                       solver->solution().data() + solver->solution().size());

        last_exit_flag = exitflag;

        return exitflag != EiCOS::exitcode::fatal;
    }

    std::string EicosWrapper::getResultString() const
    {
        switch (last_exit_flag)
        {
        case EiCOS::exitcode::optimal:
            return "Optimal solution found.";
        case EiCOS::exitcode::primal_infeasible:
            return "Certificate of primal infeasibility found.";
        case EiCOS::exitcode::dual_infeasible:
            return "Certificate of dual infeasibility found.";

        case EiCOS::exitcode::close_to_optimal:
            return "Optimal solution found subject to reduced tolerances.";
        case EiCOS::exitcode::close_to_primal_infeasible:
            return "Certificate of primal infeasibility found subject to reduced tolerances.";
        case EiCOS::exitcode::close_to_dual_infeasible:
            return "Certificate of dual infeasibility found subject to reduced tolerances.";

        case EiCOS::exitcode::maxit:
            return "Maximum number of iterations reached.";

        case EiCOS::exitcode::numerics:
            return "Numerical problems (unreliable search direction).";
        case EiCOS::exitcode::outcone:
            return "Numerical problems (slacks or multipliers outside cone)";

        default: // EiCOS::exitcode::fatal
            return "Unknown problem in solver.";
        }
    }

} // namespace op
