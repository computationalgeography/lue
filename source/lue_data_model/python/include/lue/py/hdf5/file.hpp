#pragma once
#include <string>


namespace lue {
namespace hdf5 {

unsigned int       python_mode_to_hdf5_flag(
                                        std::string const& mode);

}  // namespace hdf5
}  // namespace lue
