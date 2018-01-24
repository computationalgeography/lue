#pragma once
#include "lue/constant_collection/property_set.hpp"
#include "lue/hdf5/issues.hpp"


namespace lue {
namespace constant_collection {

void               validate            (PropertySet const& property_set,
                                        hdf5::Issues& issues);

}  // namespace constant_collection
}  // namespace lue
