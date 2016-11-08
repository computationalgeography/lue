#include "lue/cxx_api/property.h"
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
    create_soft_link(discretization.id(), "lue_space_discretization");
}


Property create_property(
    hdf5::Identifier const& location,
    std::string const& name,
    hdf5::Identifier const& domain_id,
    ValueConfiguration const& value_configuration)
{
    if(property_exists(location, name)) {
        throw std::runtime_error("Property " + name + " already exists");
    }


    hdf5::Identifier property_location(::create_property(location,
        name.c_str(), domain_id), ::close_property);

    if(!property_location.is_valid()) {
        throw std::runtime_error("Property " + name + " cannot be created");
    }

    create_value(property_location, value_configuration);

    return Property(std::move(property_location));
}

} // namespace lue
