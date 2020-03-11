#include "lue/hdf5/property_list.hpp"
#include <cassert>
#include <stdexcept>


namespace lue {
namespace hdf5 {

PropertyList::PropertyList(
    hid_t const class_id)

    : _id(::H5Pcreate(class_id), ::H5Pclose)

{
    if(!_id.is_valid()) {
        throw std::runtime_error("Cannot create property list");
    }

    assert(_id >= 0);
}


Identifier const& PropertyList::id() const
{
    return _id;
}

} // namespace hdf5
} // namespace lue
