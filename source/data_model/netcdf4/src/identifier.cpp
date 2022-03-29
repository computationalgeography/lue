#include "lue/data_model/netcdf4/identifier.hpp"
#include <cassert>


namespace lue::data_model::netcdf4 {

    Identifier::Identifier():

        _id{-1}

    {
    }


    Identifier::Identifier(
        int const id):

        _id{id}
    {
    }


    Identifier::operator int() const
    {
        return _id;
    }


    bool Identifier::is_valid() const
    {
        return _id >= 0;
    }


    void Identifier::invalidate()
    {
        assert(is_valid());

        _id = -1;

        assert(!is_valid());
    }

}  // namespace lue::data_model::netcdf4
