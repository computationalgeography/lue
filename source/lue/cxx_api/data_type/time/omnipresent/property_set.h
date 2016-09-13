#pragma once
#include "lue/cxx_api/data_type/time/omnipresent/constant_shape/property.h"
#include "lue/cxx_api/data_type/time/property_set.h"
#include "lue/cxx_api/array.h"


namespace lue {
namespace api {
namespace time {
namespace omnipresent {

/*!
    @ingroup    lue_cxx_api_group
*/
class PropertySet:
    public api::PropertySet
{

public:

                   PropertySet         (lue::PropertySet& group);

                   PropertySet         (PropertySet const& other)=delete;

                   PropertySet         (PropertySet&& other)=default;

                   ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const& other)=delete;

    PropertySet&   operator=           (PropertySet&& other)=default;

    Array&         reserve_items       (hsize_t const nr_items);

    Array&         items               ();

    constant_shape::Property&
                   add_property        (std::string const& name,
                                        hid_t const type_id,
                                        Shape const& shape,
                                        Chunks const& chunks);

private:

    std::unique_ptr<Array> _items;

    std::vector<std::unique_ptr<constant_shape::Property>>
        _constant_shape_properties;

};


void               configure_property_set(
                                        hdf5::Identifier const& location,
                                        std::string const& name,
                                        SpaceDomainConfiguration const&
                                            domain_configuration);

}  // namespace omnipresent
}  // namespace time
}  // namespace api
}  // namespace lue
