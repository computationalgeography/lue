#pragma once
#include "lue/cxx_api/chunks.h"
#include "lue/cxx_api/property_set.h"
#include "lue/cxx_api/shape.h"
#include <functional>


namespace lue {
namespace api {

/*!
    @ingroup    lue_cxx_api_group
*/
class PropertySet
{

public:

    virtual        ~PropertySet        ()=default;

    hdf5::Identifier const&
                   id                  () const;

    hdf5::Attributes const&
                   attributes          () const;

    Domain&        domain              () const;

    // virtual Property& add_property     (std::string const& name,
    //                                     hid_t const type_id,
    //                                     Shape const& shape,
    //                                     Chunks const& chunks)=0;

    Properties&    properties          () const;

protected:

                   PropertySet         (lue::PropertySet& group);

                   PropertySet         (PropertySet const& other)=delete;

                   PropertySet         (PropertySet&& other)=default;

    PropertySet&   operator=           (PropertySet const& other)=delete;

    PropertySet&   operator=           (PropertySet&& other)=default;

    lue::PropertySet const&
                   group               () const;

    lue::PropertySet&
                   group               ();

    lue::Property& add_property        (std::string const& name);

private:

    std::reference_wrapper<lue::PropertySet> _group;

};

} // namespace api
} // namespace lue
