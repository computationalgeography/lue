#pragma once
#include "lue/cxx_api/time/property_set.h"


namespace lue {
namespace omnipresent {

/*!
    @ingroup    lue_cxx_api_group
*/
class PropertySet:
    public lue::PropertySet
{

public:

                   PropertySet         (hdf5::Identifier const& location,
                                        std::string const& name);

                   PropertySet         (hdf5::Identifier&& location);

                   PropertySet         (PropertySet const& other)=delete;

                   PropertySet         (PropertySet&& other)=default;

                   ~PropertySet        ()=default;

    PropertySet&   operator=           (PropertySet const& other)=delete;

    PropertySet&   operator=           (PropertySet&& other)=default;

protected:

private:

};


PropertySet        create_property_set (hdf5::Identifier const& location,
                                        std::string const& name);

}  // namespace omnipresent
}  // namespace lue
