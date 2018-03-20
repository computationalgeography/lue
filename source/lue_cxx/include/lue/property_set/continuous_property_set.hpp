#pragma once
#include "lue/id.hpp"
#include "lue/property_set.hpp"
#include "lue/property/same_shape/continuous_property.hpp"


namespace lue {

/*!
    @brief      .
    @param      .
    @return     .
    @exception  .

    - no time domain
    - optionally set ContinuousSpaceDomain
    - optionally add one or more ContinuousProperty
*/
class ContinuousPropertySet:
    public PropertySet
{

public:

                   ContinuousPropertySet(
                                        hdf5::Identifier const& id);

                   ContinuousPropertySet(
                                        hdf5::Group& parent,
                                        std::string const& name);

                   ContinuousPropertySet(
                                        ContinuousPropertySet const&)=delete;

                   ContinuousPropertySet(
                                        ContinuousPropertySet&&)=default;

                   ~ContinuousPropertySet()=default;

    ContinuousPropertySet&
                   operator=           (ContinuousPropertySet const&)=delete;

    ContinuousPropertySet&
                   operator=           (ContinuousPropertySet&&)=default;

    // ContinuousSpaceDomain
    //                set_continuous_space_domain();

    // ContinuousProperty
    //                add_continuous_property(
    //                                    std::string const& name,
    //                                    hdf5::Datatype const& datatype);

    Id const&      id                  () const;

    Id&            id                  ();

private:

    Id             _id;

};


ContinuousPropertySet
                   create_continuous_property_set(
                                        hdf5::Group& parent,
                                        std::string const& name);

}  // namespace lue
