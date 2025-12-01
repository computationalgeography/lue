#include "lue/hdf5/property_list.hpp"
#include <cassert>
#include <stdexcept>


namespace lue::hdf5 {

    PropertyList::PropertyList(Identifier&& property_list_id):

        _id{std::move(property_list_id)}

    {
        if (!_id.is_valid())
        {
            throw std::runtime_error("Cannot create property list");
        }

        assert(_id >= 0);
    }


    auto PropertyList::id() const -> Identifier const&
    {
        return _id;
    }

}  // namespace lue::hdf5
