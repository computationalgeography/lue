#include "lue/time/property_configuration.hpp"
#include "lue/enum_string_bimap.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace time {
namespace {

detail::EnumStringBimap<PropertyConfiguration::ShapeVariability> const
        shape_variability_map = {
    { PropertyConfiguration::ShapeVariability::constant, "lue_constant_shape" }
};


std::string shape_variability_to_string(
    PropertyConfiguration::ShapeVariability const type)
{
    return shape_variability_map.as_string(type);
}


PropertyConfiguration::ShapeVariability parse_shape_variability(
    std::string const& string)
{
    if(!shape_variability_map.contains(string)) {
        throw std::runtime_error("Unknown shape variability: " + string);
    }

    return shape_variability_map.as_value(string);
}

}  // Anonymous namespace


PropertyConfiguration::PropertyConfiguration(
    PropertyConfiguration::ShapeVariability const type)

    : _shape_variability{type}

{
}


PropertyConfiguration::PropertyConfiguration(
    hdf5::Attributes const& attributes)
{
    load(attributes);
}


PropertyConfiguration::ShapeVariability
    PropertyConfiguration::shape_variability() const
{
    return _shape_variability;
}


void PropertyConfiguration::save(
    hdf5::Attributes& attributes) const
{
    attributes.write<std::string>(
        shape_variability_tag,
        shape_variability_to_string(_shape_variability)
    );
}


void PropertyConfiguration::load(
    hdf5::Attributes const& attributes)
{
    _shape_variability = parse_shape_variability(
        attributes.read<std::string>(shape_variability_tag));
}

}  // namespace time
}  // namespace lue
