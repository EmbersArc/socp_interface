#include "ecosWrapper.hpp"

static_assert(std::is_same_v<idxint, SuiteSparse_long>,
              "Definitions of idxint might not be consistent."
              "Make sure ECOS is compiled with USE_LONG = 1.");

namespace op
{
}