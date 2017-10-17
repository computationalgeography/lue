#pragma once
#include "lue/constant_size/time/omnipresent/same_shape/value.hpp"
#include "lue/property_set.hpp"


namespace lue {
namespace constant_size {

class PropertySet:
    public lue::PropertySet
{

public:

//                    PropertySet         (Phenomenon& phenomenon,
//                                         std::string const& name);

                   PropertySet         (hdf5::Identifier const& id);

                   PropertySet         (lue::PropertySet&& property_set);

                   PropertySet         (PropertySet const& other)=delete;

                   PropertySet         (PropertySet&& other)=default;

                   ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const& other)=delete;

    PropertySet&   operator=           (PropertySet&& other)=default;

    time::omnipresent::same_shape::Value const&
                   ids                 () const;

    time::omnipresent::same_shape::Value&
                   ids                 ();

    time::omnipresent::same_shape::Value&
                   reserve             (hsize_t const nr_items);

private:

    time::omnipresent::same_shape::Value _ids;

};


PropertySet        create_property_set (hdf5::Group& group,
                                        std::string const& name,
                                        Domain::Configuration const&
                                            domain_configuration);

PropertySet        create_property_set (hdf5::Group& group,
                                        std::string const& name,
                                        time::omnipresent::same_shape::Value
                                            const& ids,
                                        Domain::Configuration const&
                                            domain_configuration);

}  // namespace constant_size
}  // namespace lue
