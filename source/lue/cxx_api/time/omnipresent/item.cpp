#include "lue/cxx_api/time/omnipresent/item.h"


namespace lue {
namespace omnipresent {

Item::Item(
    hdf5::Identifier const& location,
    std::string const& name)

    : lue::Item(location, name)

{
}


Item::Item(
    hdf5::Identifier&& location)

    : lue::Item(std::forward<hdf5::Identifier>(location))

{
}

}  // namespace omnipresent
}  // namespace lue
