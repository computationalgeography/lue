#include "lue/cxx_api/property.h"
#include "lue/c_api/hdf5/hdf5_link.h"
#include "lue/c_api/property.h"
#include <cassert>
#include <stdexcept>


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
bool property_exists(
    hdf5::Identifier const& location,
    std::string const& name)
{
    return ::property_exists(location, name.c_str()) > 0;
}


Property::Property(
    hdf5::Identifier const& location,
    std::string const& name)

    : hdf5::Group(hdf5::Identifier(::open_property(location,
        name.c_str()), ::close_property))

{
    if(!id().is_valid()) {
        throw std::runtime_error("Property " + name + " cannot be opened");
    }

    assert(value_exists(id()));
    _value = std::make_unique<Value>(open_value(id()));
}


Property::Property(
    hdf5::Identifier&& location)

    : hdf5::Group(std::forward<hdf5::Identifier>(location))

{
    assert(id().is_valid());

    // Open value.
    assert(value_exists(id()));
    _value = std::make_unique<Value>(open_value(id()));
}


Value& Property::value() const
{
    assert(_value);
    return *_value;
}


void Property::link_space_discretization(
    Property const& discretization)
{
    auto status = ::hdf5_create_soft_link(discretization.id(),
       "space discretization", id());

    if(status < 0) {
        throw std::runtime_error("Space discretization cannot be linked");
    }
}


Property create_property(
    hdf5::Identifier const& location,
    std::string const& name)
{
    if(property_exists(location, name)) {
        throw std::runtime_error("Property " + name + " already exists");
    }


    hdf5::Identifier property_location(::create_property(location,
        name.c_str()), ::close_property);

    if(!property_location.is_valid()) {
        throw std::runtime_error("Property " + name + " cannot be created");
    }


    create_value(property_location);


    return Property(std::move(property_location));
}

} // namespace lue
