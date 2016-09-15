#pragma once
#include "lue/cxx_api/property_set.h"


namespace lue {
namespace time {

// bool               property_set_exists (hdf5::Identifier const& location,
//                                         std::string const& name);


/*!
    @ingroup    lue_cxx_api_group
*/
class PropertySet:
    public hdf5::Group
{

public:

    virtual        ~PropertySet        ()=default;

protected:

                   PropertySet         (hdf5::Identifier const& location,
                                        std::string const& name);

                   PropertySet         (hdf5::Identifier&& location);

                   PropertySet         (PropertySet const& other)=delete;

                   PropertySet         (PropertySet&& other)=default;

    PropertySet&   operator=           (PropertySet const& other)=delete;

    PropertySet&   operator=           (PropertySet&& other)=default;

private:

};


// hdf5::Identifier   create_property_set (hdf5::Identifier const& location,
//                                         std::string const& name);

}  // namespace time
}  // namespace lue
