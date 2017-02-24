#include "lue/cxx_api/time_discretization_configuration.h"


namespace lue {

TimeDiscretizationConfiguration::TimeDiscretizationConfiguration(
    TimeDiscretizationType const type,
    hdf5::Identifier const& property)

    : _type{type},
      _property{property}

{
}


TimeDiscretizationType TimeDiscretizationConfiguration::type() const
{
    return _type;
}


hdf5::Identifier const& TimeDiscretizationConfiguration::property() const
{
    return _property;
}

}  // namespace lue
