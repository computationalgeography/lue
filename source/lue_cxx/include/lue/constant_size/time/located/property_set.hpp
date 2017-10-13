#pragma once
#include "lue/constant_size/time/omnipresent/same_shape/value.hpp"
#include "lue/constant_size/property_set.hpp"
#include "lue/property_sets.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {

class PropertySet:
    public constant_size::PropertySet
{

public:

                   PropertySet         (hdf5::Identifier const& id);

                   // PropertySet         (constant_size::PropertySet&&
                   //                          property_set);

                   PropertySet         (PropertySet const& other)=delete;

                   PropertySet         (PropertySet&& other)=default;

                   ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const& other)=delete;

    PropertySet&   operator=           (PropertySet&& other)=default;

    omnipresent::same_shape::Value const&
                   ids                 () const;

    omnipresent::same_shape::Value&
                   ids                 ();

    omnipresent::same_shape::Value&
                   reserve             (hsize_t const nr_items);

private:

    omnipresent::same_shape::Value _ids;

};


PropertySet        create_property_set (PropertySets& property_set,
                                        std::string const& name);

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
