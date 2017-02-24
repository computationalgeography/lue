#pragma once
#include "lue/cxx_api/collection.h"
#include "lue/cxx_api/property.h"


namespace lue {

bool               properties_exists   (hdf5::Identifier const& location);


/*!
    @ingroup    lue_cxx_api_group
    @brief      Collection of properties

    The collection is modeled using an HDF5 group.
*/
template<
    typename Property=lue::Property
>
class Properties:
    public Collection<Property>
{

public:

                   Properties          (hdf5::Identifier const& location);

                   Properties          (Properties const& other)=delete;

                   Properties          (Properties&& other)=default;

                   ~Properties         ()=default;

    Properties&    operator=           (Properties const& other)=delete;

    Properties&    operator=           (Properties&& other)=default;

    Property&      add                 (std::string const& name,
                                        hdf5::Identifier const& domain_id,
                                        ValueConfiguration const&
                                            value_configuration);

private:

};


Properties<>       create_properties   (hdf5::Identifier const& location);


template<
    typename Property
>
inline Properties<Property>::Properties(
    hdf5::Identifier const& location)

    : Collection<Property>(location, "lue_properties")

{
}


template<
    typename Property
>
inline Property& Properties<Property>::add(
    std::string const& name,
    hdf5::Identifier const& domain_id,
    ValueConfiguration const& value_configuration)
{
    return Collection<Property>::add(name, create_property(
        Collection<Property>::id(), name,
        domain_id, value_configuration));
}

} // namespace lue
