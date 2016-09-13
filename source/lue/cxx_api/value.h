#pragma once
#include "lue/cxx_api/hdf5/group.h"
#include "lue/cxx_api/chunks.h"
#include "lue/cxx_api/shape.h"


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

private:

};


Value              create_value        (hdf5::Identifier const& location);

Value              open_value          (hdf5::Identifier const& location);

} // namespace lue
