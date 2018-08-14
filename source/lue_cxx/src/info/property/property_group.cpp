#include "lue/info/property/property_group.hpp"
#include "lue/core/aspect.hpp"
#include <fmt/format.h>
#include <boost/filesystem.hpp>
#include <cassert>


namespace lue {

PropertyGroup::PropertyGroup(
    hdf5::Group& parent,
    std::string const& name):

    hdf5::Group{parent, name}

{
    assert(this->name() == name);
}


PropertyGroup::PropertyGroup(
    hdf5::Group&& group):

    hdf5::Group{std::forward<hdf5::Group>(group)}

{
}


std::string PropertyGroup::name() const
{
    return id().name();
}


bool PropertyGroup::time_is_discretized() const
{
    return attributes().exists(time_discretization_tag);
}


bool PropertyGroup::space_is_discretized() const
{
    return attributes().exists(space_discretization_tag);
}


TimeDiscretization PropertyGroup::time_discretization_type() const
{
    assert(time_is_discretized());

    return Aspect<TimeDiscretization>(attributes()).value();
}


SpaceDiscretization PropertyGroup::space_discretization_type() const
{
    assert(space_is_discretized());

    return Aspect<SpaceDiscretization>(attributes()).value();
}


void PropertyGroup::set_time_discretization(
    TimeDiscretization type,
    PropertyGroup& property)
{
    Aspect<TimeDiscretization>(type).save(attributes());
    create_soft_link(property.id(), time_discretization_property_tag);
}


void PropertyGroup::set_space_discretization(
    SpaceDiscretization type,
    PropertyGroup& property)
{
    Aspect<SpaceDiscretization>(type).save(attributes());
    create_soft_link(property.id(), space_discretization_property_tag);
}


PropertyGroup PropertyGroup::time_discretization_property()
{
    assert(this->contains_soft_link(time_discretization_property_tag));

    return PropertyGroup(*this, time_discretization_property_tag);
}


PropertyGroup PropertyGroup::space_discretization_property()
{
    assert(this->contains_soft_link(space_discretization_property_tag));

    return PropertyGroup(*this, space_discretization_property_tag);
}


/*!
    @brief      Return the group representing the property set this
                property is part of
    @param      .
    @return     .
    @exception  .

    Since properties can be linked to from possibly other property sets,
    the code tries to determine the 'main' property set the property is
    part of. Not the property set of the link.
*/
hdf5::Group PropertyGroup::property_set_group()
{
    namespace bfs = boost::filesystem;
    // Path to location of the property
    bfs::path path;

    if(parent().contains_soft_link(id().name())) {
        // This property is soft linked to from some (other) property
        // set. Find the path of the property in its own property set
        path = parent().soft_link(id().name()).path();
    }
    else {
        // This property is located in its own property set
        path = id().pathname();
    }

    // The path to a property varies, depending on various factors. Split
    // the path and find the group with a name signalling the collection
    // of property sets. This group contains the group we are looking for.
    std::vector<std::string> names;
    std::transform(path.begin(), path.end(), std::back_inserter(names),
            [](bfs::path path) { return path.string(); }
        );

    // Determine number of groups between property and property set level
    // position iterator points to either name that equals
    // property_sets_tag or collection_property_sets_tag
    auto const position = std::find_if(names.begin(), names.end(),
            [](auto const& name) {
                return
                    name == property_sets_tag ||
                    name == collection_property_sets_tag; }
        );

    if(position == names.end()) {
        // This should never happen
        throw std::logic_error(fmt::format(
                "Could not determine property set collection of {}",
                path.string()
            ));
    }

    std::size_t const nr_groups = std::distance(position, names.end()) - 2;

    hdf5::Group parent = this->parent();

    for(std::size_t i = 0; i < nr_groups; ++i) {
        parent = std::move(parent.parent());
    }

    // We are at the property set level
    auto property_set_group = std::move(parent);

    if(property_set_group.parent().id().name() != *position) {
        // A phenomenon has two property sets: one for the information per
        // object and one for the information for the collection of objects.
        // The property we are looking for is located in the other
        // property set than the property we are in.

        // For now we are assuming we are in property_sets and the
        // property we are looking for is in collection_property_sets

        // From phenomenon group, obtain collection property sets group
        hdf5::Group property_sets_group{
            property_set_group.parent().parent(), collection_property_sets_tag};

        // From property sets group, obtain property group
        auto const property_set_name = *(position + 1);
        assert(property_sets_group.contains_group(property_set_name));
        property_set_group = hdf5::Group{property_sets_group, property_set_name};
    }

    // property_set_group is the group of the property set. Its parent
    // is a collection of property sets.
    assert(property_set_group.parent().id().name() == *position);

    return property_set_group;
}


PropertyGroup create_property_group(
    hdf5::Group& parent,
    std::string const& name)
{
    auto group = hdf5::create_group(parent, name);

    return PropertyGroup{std::move(group)};
}

}  // namespace lue
