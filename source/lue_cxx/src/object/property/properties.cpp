#include "lue/object/property/properties.hpp"
#include "lue/core/tag.hpp"
#include <boost/optional.hpp>
#include <fmt/core.h>


namespace lue {

Properties::Properties(
    hdf5::Group const& parent):

    hdf5::Group{parent, properties_tag},
    _same_shape_properties{*this},
    _same_shape_constant_shape_properties{*this},
    _same_shape_variable_shape_properties{*this},
    _different_shape_properties{*this},
    _different_shape_constant_shape_properties{*this},
    _different_shape_variable_shape_properties{*this}

{
}


Properties::Properties(
    hdf5::Group&& group):

    hdf5::Group{std::forward<hdf5::Group>(group)},
    _same_shape_properties{*this},
    _same_shape_constant_shape_properties{*this},
    _same_shape_variable_shape_properties{*this},
    _different_shape_properties{*this},
    _different_shape_constant_shape_properties{*this},
    _different_shape_variable_shape_properties{*this}

{
}


bool Properties::empty() const
{
    return size() == 0;
}


std::size_t Properties::size() const
{
    return
        _same_shape_properties.size() +
        _same_shape_constant_shape_properties.size() +
        _same_shape_variable_shape_properties.size() +
        _different_shape_properties.size() +
        _different_shape_constant_shape_properties.size() +
        _different_shape_variable_shape_properties.size()
        ;
}


bool Properties::contains(
    std::string const& name) const
{
    return
        _same_shape_properties.contains(name) ||
        _same_shape_constant_shape_properties.contains(name) ||
        _same_shape_variable_shape_properties.contains(name) ||
        _different_shape_properties.contains(name) ||
        _different_shape_constant_shape_properties.contains(name) ||
        _different_shape_variable_shape_properties.contains(name)
        ;
}


ShapePerObject Properties::shape_per_object(
    std::string const& name) const
{
    boost::optional<ShapePerObject> result;

    if(     _same_shape_properties.contains(name) ||
            _same_shape_constant_shape_properties.contains(name) ||
            _same_shape_variable_shape_properties.contains(name)) {

        result = ShapePerObject::same;

    }
    else if(_different_shape_properties.contains(name) ||
            _different_shape_constant_shape_properties.contains(name) ||
            _different_shape_variable_shape_properties.contains(name)) {

        result = ShapePerObject::different;

    }

    if(!result) {
        throw std::runtime_error(fmt::format(
                "Property named {} does not exist",
                name
            ));
    }

    return *result;
}


ValueVariability Properties::value_variability(
    std::string const& name) const
{
    boost::optional<ValueVariability> result;

    if(     _same_shape_properties.contains(name) ||
            _different_shape_properties.contains(name)) {

        result = ValueVariability::constant;

    }
    else if(_same_shape_constant_shape_properties.contains(name) ||
            _same_shape_variable_shape_properties.contains(name) ||
            _different_shape_constant_shape_properties.contains(name) ||
            _different_shape_variable_shape_properties.contains(name)) {

        result = ValueVariability::variable;

    }

    if(!result) {
        throw std::runtime_error(fmt::format(
                "Property named {} does not exist",
                name
            ));
    }

    return *result;
}


ShapeVariability Properties::shape_variability(
    std::string const& name) const
{
    boost::optional<ShapeVariability> result;

    if(
            _same_shape_constant_shape_properties.contains(name) ||
            _different_shape_constant_shape_properties.contains(name)) {

        result = ShapeVariability::constant;

    }
    else if(_same_shape_variable_shape_properties.contains(name) ||
            _different_shape_variable_shape_properties.contains(name)) {

        result = ShapeVariability::variable;

    }

    if(!result) {
        throw std::runtime_error(fmt::format(
                "Property named {} does not exist or does not vary through time",
                name
            ));
    }

    return *result;
}


same_shape::Property& Properties::add(
    std::string const& name,
    hdf5::Datatype const& datatype)
{
    if(contains(name)) {
        throw std::runtime_error(fmt::format(
                "Property named {} already exists",
                name
            ));
    }

    return _same_shape_properties.add(name, datatype);
}


Properties create_properties(
    hdf5::Group& parent,
    ObjectTracker& object_tracker)
{
    // Pass the object ID tracking stuff to the
    // specialized Properties classes. They will hard-link to it. That
    // way this information can be shared.
    auto group = hdf5::create_group(parent, properties_tag);

    auto& id = object_tracker.id();
    auto& active_id = object_tracker.active_id();
    auto& active_set_index = object_tracker.active_set_index();
    auto& active_object_index = object_tracker.active_object_index();

    same_shape::create_properties(group, id);
    same_shape::constant_shape::create_properties(
        group, active_id, active_set_index);
    same_shape::variable_shape::create_properties(
        group, active_id, active_set_index);
    different_shape::create_properties(group, id);
    different_shape::constant_shape::create_properties(
        group, active_id, active_set_index, active_object_index);
    different_shape::variable_shape::create_properties(
        group);

    return Properties{std::move(group)};
}

}  // namespace lue
