#pragma once
#include "lue/constant_size/property_set.hpp"
#include "lue/hdf5/issues.hpp"


namespace lue {
namespace constant_size {

void               validate            (PropertySet const& property_set,
                                        hdf5::Issues& issues);

}  // namespace constant_size
}  // namespace lue
