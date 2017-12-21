#pragma once
#include "lue/item/constant_size/constant/same_shape/collection.hpp"
#include "lue/property_set.hpp"


namespace lue {
namespace constant_size {

class PropertySet:
    public lue::PropertySet
{

public:

    using Ids = constant::same_shape::Collection;

                   PropertySet         (hdf5::Identifier const& id);

                   PropertySet         (lue::PropertySet&& property_set);

                   PropertySet         (PropertySet const&)=delete;

                   PropertySet         (PropertySet&&)=default;

                   ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const&)=delete;

    PropertySet&   operator=           (PropertySet&&)=default;

    Ids const&     ids                 () const;

    Ids&           ids                 ();

private:

    //! Collection of item-ids
    constant::same_shape::Collection _ids;

};


PropertySet        create_property_set (hdf5::Group& group,
                                        std::string const& name,
                                        Domain::Configuration const&
                                            domain_configuration);

PropertySet        create_property_set (hdf5::Group& group,
                                        std::string const& name,
                                        PropertySet::Ids const& ids,
                                        Domain::Configuration const&
                                            domain_configuration);

}  // namespace constant_size
}  // namespace lue
