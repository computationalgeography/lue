#pragma once
#include "lue/cxx_api/chunks.h"
#include "lue/cxx_api/property_set.h"
#include "lue/cxx_api/shape.h"
#include <functional>


namespace lue {
namespace constant_size {
namespace time {

/*!
    @ingroup    lue_cxx_api_group

    Base class for instances providing a more specialized API to
    manipulating a property set. That more specialized API is dependent
    on the time domain configuration.

    This class layers the more general lue::PropertySet for functionality
    common to all property sets.
*/
class PropertySet
{

public:

                   PropertySet         (lue::PropertySet& group);

                   PropertySet         (PropertySet const& other)=delete;

                   PropertySet         (PropertySet&& other)=default;

    virtual        ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const& other)=delete;

    PropertySet&   operator=           (PropertySet&& other)=default;

    hdf5::Identifier const&
                   id                  () const;

    hdf5::Attributes const&
                   attributes          () const;

    PropertySetConfiguration const&
                   configuration       () const;

    Domain&        domain              () const;

    Properties&    properties          () const;

protected:

    lue::PropertySet const&
                   group               () const;

    lue::PropertySet&
                   group               ();

    lue::Property& add_property        (std::string const& name,
                                        ValueConfiguration const&
                                            value_configuration);

private:

    std::reference_wrapper<lue::PropertySet> _group;

};

} // namespace time
}  // namespace constant_size
} // namespace lue
