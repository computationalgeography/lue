#include "lue/constant_size/validate.hpp"
#include "lue/constant_size/time/omnipresent/validate.hpp"


namespace lue {
namespace constant_size {

void validate(
    PropertySet const& property_set,
    hdf5::Issues& issues)
{
    time::omnipresent::validate(
        time::omnipresent::PropertySet(property_set.id()), issues);
}

}  // namespace constant_size
}  // namespace lue
