#pragma once
#include "lue/constant_collection/property_set.hpp"
#include "lue/hdf5/issues.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace located {

void               validate            (PropertySet const& property_set,
                                        hdf5::Issues& issues);

}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
