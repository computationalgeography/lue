#include "lue/cxx_api/value_configuration.h"
#include "lue/cxx_api/enum_string_bimap.h"


namespace lue {
namespace {

detail::EnumStringBimap<ShapeThroughTime> const shape_through_time_map = {
    { ShapeThroughTime::constant, "constant" },
    { ShapeThroughTime::variable, "variable" }
};

detail::EnumStringBimap<ShapePerItem> const shape_per_item_map = {
    { ShapePerItem::same, "same" },
    { ShapePerItem::different, "different" }
};

}  // Anonymous namespace


/*!
    @ingroup    lue_cxx_api_group
*/
std::string shape_through_time_to_string(
    ShapeThroughTime const shape_through_time)
{
    return shape_through_time_map.as_string(shape_through_time);
}


/*!
    @ingroup    lue_cxx_api_group
*/
ShapeThroughTime parse_shape_through_time(
    std::string const& string)
{
    if(!shape_through_time_map.contains(string)) {
        throw std::runtime_error("Unknown shape through time: " + string);
    }

    return shape_through_time_map.as_value(string);
}


/*!
    @ingroup    lue_cxx_api_group
*/
std::string shape_per_item_to_string(
    ShapePerItem const shape_per_item)
{
    return shape_per_item_map.as_string(shape_per_item);
}


/*!
    @ingroup    lue_cxx_api_group
*/
ShapePerItem parse_shape_per_item(
    std::string const& string)
{
    if(!shape_per_item_map.contains(string)) {
        throw std::runtime_error("Unknown shape per item: " + string);
    }

    return shape_per_item_map.as_value(string);
}


ValueConfiguration::ValueConfiguration(
    ShapeThroughTime const shape_through_time,
    ShapePerItem const shape_per_item)

    : _shape_through_time{shape_through_time},
      _shape_per_item{shape_per_item}

{
}


ShapeThroughTime ValueConfiguration::shape_through_time() const
{
    return _shape_through_time;
}

ShapePerItem ValueConfiguration::shape_per_item() const
{
    return _shape_per_item;
}

}  // namespace lue
