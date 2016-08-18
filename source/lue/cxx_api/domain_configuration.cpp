#include "lue/cxx_api/domain_configuration.h"


namespace lue {

DomainConfiguration::DomainConfiguration()

    : _time{},
      _space{}

{
}


DomainConfiguration::DomainConfiguration(
    TimeDomainConfiguration const& time)

    : _time{time},
      _space{}

{
}


DomainConfiguration::DomainConfiguration(
    SpaceDomainConfiguration const& space)

    : _time{},
      _space{space}

{
}


DomainConfiguration::DomainConfiguration(
    TimeDomainConfiguration const& time,
    SpaceDomainConfiguration const& space)

    : _time{time},
      _space{space}

{
}


TimeDomainConfiguration const& DomainConfiguration::time() const
{
    return _time;
}


SpaceDomainConfiguration const& DomainConfiguration::space() const
{
    return _space;
}

}  // namespace lue
