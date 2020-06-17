#pragma once
#include <string>


namespace lue {
namespace hdf5 {

unsigned int       python_mode_to_hdf5_flag(
                                        std::string const& mode);

std::string        intent_to_python_mode(
                                        unsigned int intent);

}  // namespace hdf5
}  // namespace lue
