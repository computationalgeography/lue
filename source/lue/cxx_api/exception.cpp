#include "lue/cxx_api/exception.h"
#include <stdexcept>


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
void throw_unsupported_error(
    std::string const& feature)
{
    throw std::runtime_error("Unsupported feature: " + feature);
}

}  // namespace lue
