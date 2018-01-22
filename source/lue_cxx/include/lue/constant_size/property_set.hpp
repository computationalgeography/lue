#pragma once
#include "lue/item/constant_size/constant_shape/same_shape/constant.hpp"
#include "lue/property_set.hpp"


namespace lue {
namespace constant_size {

class PropertySet:
    public lue::PropertySet
{

public:

    using Ids = constant_shape::same_shape::Constant;

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
    constant_shape::same_shape::Constant _ids;

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
