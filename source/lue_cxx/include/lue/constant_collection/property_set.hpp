#pragma once
#include "lue/item/constant_shape/same_shape/continuous_value.hpp"
#include "lue/phenomenon.hpp"


namespace lue {
namespace constant_collection {

class PropertySet:
    public lue::PropertySet
{

public:

    using Ids = lue::constant_shape::same_shape::ContinuousValue;

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
    Ids            _ids;

};


PropertySet        create_property_set (Phenomenon& phenomenon,
                                        std::string const& name);

PropertySet        create_property_set (Phenomenon& phenomenon,
                                        std::string const& name,
                                        PropertySet::Ids const& ids);

}  // namespace constant_collection
}  // namespace lue
