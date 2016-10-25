#include "lue/cxx_api/value.h"
#include "lue/c_api/value.h"
#include <stdexcept>


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
bool value_exists(
    hdf5::Identifier const& location)
{
    return ::value_exists(location) > 0;
}


Value::Value(
    hdf5::Identifier&& location)

    : hdf5::Group(std::forward<hdf5::Identifier>(location)),
      _configuration{ValueConfiguration(
          parse_shape_per_item_type(
              attributes().read<std::string>("lue_shape_per_item_type")))
      }

{
}


ValueConfiguration const& Value::configuration() const
{
    return _configuration;
}


/*!
    @ingroup    lue_cxx_api_group
*/
Value create_value(
    hdf5::Identifier const& location,
    ValueConfiguration const& configuration)
{
    if(value_exists(location)) {
        throw std::runtime_error("Value already exists");
    }

    hdf5::Identifier value_location(::create_value(location), ::close_value);

    if(!value_location.is_valid()) {
        throw std::runtime_error("Value cannot be created");
    }

    hdf5::Attributes value_attributes(value_location);

    value_attributes.write<std::string>("lue_shape_per_item_type",
        shape_per_item_type_to_string(configuration.shape_per_item_type()));

    return Value(std::move(value_location));
}


Value open_value(
    hdf5::Identifier const& location)
{
    hdf5::Identifier value_location(::open_value(location), ::close_value);

    if(!value_location.is_valid()) {
        throw std::runtime_error("Cannot open property value");
    }

    return Value(std::move(value_location));
}

} // namespace lue
