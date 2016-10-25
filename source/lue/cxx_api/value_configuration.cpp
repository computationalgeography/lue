#include "lue/cxx_api/value_configuration.h"
#include "lue/cxx_api/enum_string_bimap.h"


namespace lue {
namespace {

detail::EnumStringBimap<ShapePerItemType> const shape_per_item_type_map = {
    { ShapePerItemType::same_shape, "same_shape" },
    { ShapePerItemType::different_shape, "different_shape" }
};

}  // Anonymous namespace


/*!
    @ingroup    lue_cxx_api_group
*/
std::string shape_per_item_type_to_string(
    ShapePerItemType const type)
{
    return shape_per_item_type_map.as_string(type);
}


/*!
    @ingroup    lue_cxx_api_group
*/
ShapePerItemType parse_shape_per_item_type(
    std::string const& string)
{
    if(!shape_per_item_type_map.contains(string)) {
        throw std::runtime_error("Unknown shape per item type: " + string);
    }

    return shape_per_item_type_map.as_value(string);
}


ValueConfiguration::ValueConfiguration(
    ShapePerItemType const type)

    : _shape_per_item_type{type}

{
}


ShapePerItemType ValueConfiguration::shape_per_item_type() const
{
    return _shape_per_item_type;
}

}  // namespace lue
