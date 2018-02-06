#pragma once
#include "lue/item/constant_collection/constant_shape/same_shape/constant.hpp"
#include "lue/phenomenon.hpp"


namespace lue {
namespace constant_collection {

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


PropertySet        create_property_set (Phenomenon& phenomenon,
                                        std::string const& name);

PropertySet        create_property_set (Phenomenon& phenomenon,
                                        std::string const& name,
                                        PropertySet::Ids const& ids);

}  // namespace constant_collection
}  // namespace lue
