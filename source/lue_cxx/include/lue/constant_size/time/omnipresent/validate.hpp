#pragma once
#include "lue/constant_size/time/omnipresent/property_set.hpp"
#include "lue/hdf5/issues.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

void               validate            (PropertySet const& property_set,
                                        hdf5::Issues& issues);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
