#pragma once
#include "lue/hdf5/group.hpp"


namespace lue {
namespace constant_shape {

hdf5::Group        create_group        (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        int rank);

hdf5::Group        create_group        (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Shape const& value_shape);

}  // namespace constant_shape
}  // namespace lue
