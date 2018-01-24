#pragma once
#include "lue/constant_collection/time/omnipresent/property_set.hpp"
#include "lue/hdf5/issues.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {

void               validate            (PropertySet const& property_set,
                                        hdf5::Issues& issues);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
