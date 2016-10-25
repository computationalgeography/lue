#pragma once
#include "lue/cxx_api/hdf5/group.h"
#include "lue/cxx_api/value.h"
#include <memory>


namespace lue {

bool               property_exists     (hdf5::Identifier const& location,
                                        std::string const& name);


/*!
    @ingroup    lue_cxx_api_group
*/
class Property:
    public hdf5::Group
{

public:

                   Property            (hdf5::Identifier const& location,
                                        std::string const& name);

                   Property            (hdf5::Identifier&& location);

                   Property            (Property const& other)=delete;

                   Property            (Property&& other)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const& other)=delete;

    Property&      operator=           (Property&& other)=default;

    Value&         value               () const;

    // void           link_space_discretization(
    //                                     Property const& discretization);

private:

    std::unique_ptr<Value> _value;

};


Property           create_property     (hdf5::Identifier const& location,
                                        std::string const& name,
                                        hdf5::Identifier const& domain_id,
                                        ValueConfiguration const&
                                            value_configuration);

} // namespace lue
