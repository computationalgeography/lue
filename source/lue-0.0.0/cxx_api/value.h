#pragma once
#include "lue/cxx_api/chunks.h"
#include "lue/cxx_api/shape.h"
#include "lue/cxx_api/value_configuration.h"
#include "lue/cxx_api/hdf5/group.h"


namespace lue {

bool               value_exists        (hdf5::Identifier const& location);


/*!
    @ingroup    lue_cxx_api_group
*/
class Value:
    public hdf5::Group
{

public:

                   Value               (hdf5::Identifier&& location);

                   Value               (Value const& other)=delete;

                   Value               (Value&& other)=default;

                   ~Value              ()=default;

    Value&         operator=           (Value const& other)=delete;

    Value&         operator=           (Value&& other)=default;

    ValueConfiguration const&
                   configuration       () const;

private:

    ValueConfiguration
                   _configuration;

};


Value              create_value        (hdf5::Identifier const& location,
                                        ValueConfiguration const&
                                            configuration);

Value              open_value          (hdf5::Identifier const& location);

} // namespace lue
