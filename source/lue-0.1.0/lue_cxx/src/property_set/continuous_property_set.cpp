#include "lue/property_set/continuous_property_set.hpp"


namespace lue {

ContinuousPropertySet::ContinuousPropertySet(
    hdf5::Identifier const& id)

    : PropertySet{id},
      _id{*this}

{
}


ContinuousPropertySet::ContinuousPropertySet(
    hdf5::Group& parent,
    std::string const& name)

    : PropertySet{parent, name},
      _id{*this}

{
}


Id const& ContinuousPropertySet::id() const
{
    return _id;
}


Id& ContinuousPropertySet::id()
{
    return _id;
}


ContinuousPropertySet create_continuous_property_set(
    hdf5::Group& parent,
    std::string const& name)
{
    auto property_set = create_property_set(
        parent, name,
        PropertySetConfiguration{
            Occurrence::continuous
        }
    );

    create_id(property_set);

    return ContinuousPropertySet{parent, name};
}

}  // namespace lue
