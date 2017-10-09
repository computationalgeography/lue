#pragma once
#include "lue/constant_size/time/omnipresent/same_shape/value.hpp"
#include "lue/constant_size/time/omnipresent/domain.hpp"
#include "lue/constant_size/property_set.hpp"
#include "lue/phenomenon.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

/*!
    @brief      TODO
    @sa         create_property_set(Phenomenon&, std::string const&)
*/
class PropertySet:
    public constant_size::PropertySet
{

public:

//                    PropertySet         (Phenomenon& phenomenon,
//                                         std::string const& name);

                   // PropertySet         (constant_size::PropertySet&&
                   //                          property_set);

                   PropertySet         (hdf5::Identifier const& id);

                   PropertySet         (PropertySet const& other)=delete;

                   PropertySet         (PropertySet&& other)=default;

                   ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const& other)=delete;

    PropertySet&   operator=           (PropertySet&& other)=default;

    same_shape::Value const&
                   ids                 () const;

    same_shape::Value&
                   ids                 ();

    Domain const&  domain              () const;

    Domain&        domain              ();

    same_shape::Value&
                   reserve             (hsize_t const nr_items);

private:

    Domain         _domain;

    same_shape::Value _ids;

};


PropertySet        create_property_set (Phenomenon& phenomenon,
                                        std::string const& name);

PropertySet        create_property_set (Phenomenon& phenomenon,
                                        std::string const& name,
                                        same_shape::Value const& ids);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
