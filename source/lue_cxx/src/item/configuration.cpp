#include "lue/item/configuration.hpp"
#include "lue/enum_string_bimap.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace {

detail::EnumStringBimap<Configuration::CollectionVariability> const
        collection_variability_map = {
    { Configuration::CollectionVariability::constant,
        "lue_constant_collection" }
};


std::string collection_variability_to_string(
    Configuration::CollectionVariability const type)
{
    return collection_variability_map.as_string(type);
}


Configuration::CollectionVariability parse_collection_variability(
    std::string const& string)
{
    if(!collection_variability_map.contains(string)) {
        throw std::runtime_error("Unknown collection variability: " + string);
    }

    return collection_variability_map.as_value(string);
}


detail::EnumStringBimap<Configuration::ShapeVariability> const
        shape_variability_map = {
    { Configuration::ShapeVariability::constant, "lue_constant_shape" }
};


std::string shape_variability_to_string(
    Configuration::ShapeVariability const type)
{
    return shape_variability_map.as_string(type);
}


Configuration::ShapeVariability parse_shape_variability(
    std::string const& string)
{
    if(!shape_variability_map.contains(string)) {
        throw std::runtime_error("Unknown shape variability: " + string);
    }

    return shape_variability_map.as_value(string);
}

}  // Anonymous namespace


Configuration::Configuration(
    CollectionVariability const collection_variability,
    ShapeVariability const shape_variability)

    : _collection_variability{collection_variability},
      _shape_variability{shape_variability}

{
}


Configuration::Configuration(
    hdf5::Attributes const& attributes)
{
    load(attributes);
}


Configuration::CollectionVariability
    Configuration::collection_variability() const
{
    return _collection_variability;
}


Configuration::ShapeVariability
    Configuration::shape_variability() const
{
    return _shape_variability;
}


void Configuration::save(
    hdf5::Attributes& attributes) const
{
    attributes.write<std::string>(
        collection_variability_tag,
        collection_variability_to_string(_collection_variability)
    );
    attributes.write<std::string>(
        shape_variability_tag,
        shape_variability_to_string(_shape_variability)
    );
}


void Configuration::load(
    hdf5::Attributes const& attributes)
{
    _collection_variability = parse_collection_variability(
        attributes.read<std::string>(collection_variability_tag));
    _shape_variability = parse_shape_variability(
        attributes.read<std::string>(shape_variability_tag));
}

}  // namespace lue
