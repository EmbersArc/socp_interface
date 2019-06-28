#include "ecosWrapper.hpp"

#if(MOSEK_AVAILABLE)
    #include "mosekWrapper.hpp"
#endif

using Solver = EcosWrapper;
// using Solver = MosekWrapper;
