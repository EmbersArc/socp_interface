#include "parameter.hpp"

#include <sstream>

namespace op
{

std::string Parameter::print() const
{
    std::ostringstream s;
    s << "(" << get_value() << ")";
    return s.str();
}

} // namespace op