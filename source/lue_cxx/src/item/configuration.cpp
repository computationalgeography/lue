#include "lue/item/configuration.hpp"
#include "lue/enum_string_bimap.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace {

detail::EnumStringBimap<CollectionVariability> const
        collection_variability_map = {
    { CollectionVariability::constant,
        "lue_constant_collection" }
};


std::string collection_variability_to_string(
    CollectionVariability const type)
{
    return collection_variability_map.as_string(type);
}


CollectionVariability parse_collection_variability(
    std::string const& string)
{
    if(!collection_variability_map.contains(string)) {
        throw std::runtime_error("Unknown collection variability: " + string);
    }

    return collection_variability_map.as_value(string);
}


detail::EnumStringBimap<ShapeVariability> const
        shape_variability_map = {
    { ShapeVariability::constant, "lue_constant_shape" }
};


std::string shape_variability_to_string(
    ShapeVariability const type)
{
    return shape_variability_map.as_string(type);
}


ShapeVariability parse_shape_variability(
    std::string const& string)
{
    if(!shape_variability_map.contains(string)) {
        throw std::runtime_error("Unknown shape variability: " + string);
    }

    return shape_variability_map.as_value(string);
}


detail::EnumStringBimap<ShapePerItem> const
        shape_per_item_map = {
    { ShapePerItem::same, "lue_same_shape" },
    { ShapePerItem::different, "lue_different_shape" }
};


std::string shape_per_item_to_string(
    ShapePerItem const type)
{
    return shape_per_item_map.as_string(type);
}


ShapePerItem parse_shape_per_item(
    std::string const& string)
{
    if(!shape_per_item_map.contains(string)) {
        throw std::runtime_error("Unknown shape per item: " + string);
    }

    return shape_per_item_map.as_value(string);
}


detail::EnumStringBimap<ValueVariability> const value_variability_map = {
    { ValueVariability::constant, "lue_constant_value" },
    { ValueVariability::variable, "lue_variable_value" }
};


std::string value_variability_to_string(
    ValueVariability const type)
{
    return value_variability_map.as_string(type);
}


ValueVariability parse_value_variability(
    std::string const& string)
{
    if(!value_variability_map.contains(string)) {
        throw std::runtime_error("Unknown value variability: " + string);
    }

    return value_variability_map.as_value(string);
}

}  // Anonymous namespace


Configuration::Configuration(
    CollectionVariability const collection_variability,
    ShapeVariability const shape_variability,
    ShapePerItem const shape_per_item,
    ValueVariability const value_variability)

    : _collection_variability{collection_variability},
      _shape_variability{shape_variability},
      _shape_per_item{shape_per_item},
      _value_variability{value_variability}

{
}


Configuration::Configuration(
    hdf5::Attributes const& attributes)
{
    load(attributes);
}


CollectionVariability Configuration::collection_variability() const
{
    return _collection_variability;
}


ShapeVariability Configuration::shape_variability() const
{
    return _shape_variability;
}


ShapePerItem Configuration::shape_per_item() const
{
    return _shape_per_item;
}


ValueVariability Configuration::value_variability() const
{
    return _value_variability;
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
    attributes.write<std::string>(
        shape_per_item_tag,
        shape_per_item_to_string(_shape_per_item)
    );
    attributes.write<std::string>(
        value_variability_tag,
        value_variability_to_string(_value_variability)
    );
}


void Configuration::load(
    hdf5::Attributes const& attributes)
{
    _collection_variability = parse_collection_variability(
        attributes.read<std::string>(collection_variability_tag));
    _shape_variability = parse_shape_variability(
        attributes.read<std::string>(shape_variability_tag));
    _shape_per_item = parse_shape_per_item(
        attributes.read<std::string>(shape_per_item_tag));
    _value_variability = parse_value_variability(
        attributes.read<std::string>(value_variability_tag));
}

}  // namespace lue
