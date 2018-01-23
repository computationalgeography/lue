#pragma once
#include "lue/hdf5/group.hpp"


namespace lue {
namespace constant_size {
namespace constant_shape {
namespace different_shape {

hdf5::Group        create_group        (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace constant_size
}  // namespace lue
