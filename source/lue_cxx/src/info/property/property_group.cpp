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


bool PropertyGroup::space_is_discretized() const
{
    return attributes().exists(space_discretization_tag);
}


SpaceDiscretization PropertyGroup::space_discretization_type() const
{
    assert(space_is_discretized());

    return Aspect<SpaceDiscretization>(attributes()).value();
}


void PropertyGroup::set_space_discretisation(
    SpaceDiscretization type,
    PropertyGroup& property)
{
    Aspect<SpaceDiscretization>(type).save(attributes());
    create_soft_link(property.id(), space_discretization_property_tag);
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
    auto position = std::find_if(names.begin(), names.end(),
            [](auto const& name) {
                return
                    name == property_sets_tag ||
                    name == collection_property_sets_tag; }
        );

    if(position == names.end()) {
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

    // parent is the group of the property set. Its parent is a collection
    // of property sets.
    assert(
        parent.parent().id().name() == property_sets_tag ||
        parent.parent().id().name() == collection_property_sets_tag);

    return parent;
}


PropertyGroup create_property_group(
    hdf5::Group& parent,
    std::string const& name)
{
    auto group = hdf5::create_group(parent, name);

    return PropertyGroup{std::move(group)};
}

}  // namespace lue
