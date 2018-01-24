#include "lue/constant_collection/validate.hpp"
#include "lue/constant_collection/time/located/validate.hpp"
#include "lue/constant_collection/time/omnipresent/validate.hpp"


namespace lue {
namespace constant_collection {

void validate(
    PropertySet const& property_set,
    hdf5::Issues& issues)
{
    auto const& configuration = property_set.domain().configuration();

    switch(configuration.domain_type()) {

        case(Domain::Configuration::DomainType::omnipresent): {
            time::omnipresent::validate(
                time::omnipresent::PropertySet(
                    property_set.id()), issues);
            break;
        }

        case(Domain::Configuration::DomainType::located): {
            time::located::validate(
                time::located::PropertySet(
                    hdf5::Group{property_set.id()}), issues);
            break;
        }

    }
}

}  // namespace constant_collection
}  // namespace lue
