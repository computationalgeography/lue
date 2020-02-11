#include "view_configuration.hpp"


namespace lue {
namespace utility {

ViewConfiguration::ViewConfiguration():

    _show_details{false}

{
}


bool ViewConfiguration::show_details() const
{
    return _show_details;
}


bool& ViewConfiguration::show_details()
{
    return _show_details;
}

}  // namespace utility
}  // namespace lue
