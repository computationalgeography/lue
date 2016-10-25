#pragma once
#include "lue/cxx_api/hdf5/identifier.h"
#include "lue/cxx_api/define.h"


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
class TimeDiscretizationConfiguration
{

public:

                   TimeDiscretizationConfiguration(
                                        TimeDiscretizationType const type,
                                        hdf5::Identifier const& property);

                   ~TimeDiscretizationConfiguration()=default;

    TimeDiscretizationType
                   type                () const;

    hdf5::Identifier const&
                   property            () const;

private:

    TimeDiscretizationType _type;

    hdf5::Identifier const& _property;

};

}  // namespace lue
