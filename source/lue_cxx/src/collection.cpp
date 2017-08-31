#include "lue/collection.hpp"


namespace lue {

bool collection_exists(
    hdf5::Identifier const& location,
    std::string const& name)
{
    return hdf5::group_exists(location, name);
}

} // namespace lue
