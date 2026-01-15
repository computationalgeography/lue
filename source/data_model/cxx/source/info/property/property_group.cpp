#include "lue/info/property/property_group.hpp"
#include "lue/core/aspect.hpp"
#include <cassert>
#include <filesystem>


namespace lue::data_model {

    PropertyGroup::PropertyGroup(hdf5::Group const& parent, std::string const& name):

        hdf5::Group{parent, name},
        _description{
            attributes().exists(description_tag) ? attributes().read<std::string>(description_tag) : ""}

    {
        assert(this->name() == std::filesystem::path{name}.filename());
    }


    PropertyGroup::PropertyGroup(hdf5::Group&& group):

        hdf5::Group{std::move(group)},
        _description{
            attributes().exists(description_tag) ? attributes().read<std::string>(description_tag) : ""}

    {
    }


    auto PropertyGroup::name() const -> std::string
    {
        return id().name();
    }


    auto PropertyGroup::description() const -> std::string const&
    {
        return _description;
    }


    auto PropertyGroup::time_is_discretized() const -> bool
    {
        return attributes().exists(time_discretization_tag);
    }


    auto PropertyGroup::space_is_discretized() const -> bool
    {
        return attributes().exists(space_discretization_tag);
    }


    auto PropertyGroup::time_discretization_type() const -> TimeDiscretization
    {
        assert(time_is_discretized());

        return Aspect<TimeDiscretization>(attributes()).value();
    }


    auto PropertyGroup::space_discretization_type() const -> SpaceDiscretization
    {
        assert(space_is_discretized());

        return Aspect<SpaceDiscretization>(attributes()).value();
    }


    /*!
        @brief      Link a property containing information about the discretization through time of this
                    property's value
        @param      type Kind of discretization
        @param      property Property containing discretization information
        @warning    It is assumed here that this property's value varies through time
        @todo       Can we change the API to enforce the precondition that this property is temporal?
    */
    void PropertyGroup::set_time_discretization(TimeDiscretization type, PropertyGroup& property)
    {
        Aspect<TimeDiscretization>(type).save(attributes());
        create_soft_link(property.id(), time_discretization_property_tag);
    }


    /*!
        @brief      Link a property containing information about the discretization through space of this
                    property's value
        @param      type Kind of discretization
        @param      property Property containing discretization information
        @warning    It is assumed here that this property's value varies through space
        @todo       Can we change the API to enforce the precondition that this property is spatial?
    */
    void PropertyGroup::set_space_discretization(SpaceDiscretization type, PropertyGroup& property)
    {
        Aspect<SpaceDiscretization>(type).save(attributes());
        create_soft_link(property.id(), space_discretization_property_tag);
    }


    auto PropertyGroup::time_discretization_property() -> PropertyGroup
    {
        assert(this->contains_soft_link(time_discretization_property_tag));

        return PropertyGroup{*this, hdf5::SoftLink{*this, time_discretization_property_tag}.path()};
    }


    auto PropertyGroup::space_discretization_property() -> PropertyGroup
    {
        assert(this->contains_soft_link(space_discretization_property_tag));

        return PropertyGroup{*this, hdf5::SoftLink{*this, space_discretization_property_tag}.path()};
    }


    auto create_property_group(hdf5::Group& parent, std::string const& name, std::string const& description)
        -> PropertyGroup
    {
        hdf5::Group group{hdf5::create_group(parent, name)};

        if (!description.empty())
        {
            group.attributes().write(description_tag, description);
        }

        return PropertyGroup{std::move(group)};
    }

}  // namespace lue::data_model
