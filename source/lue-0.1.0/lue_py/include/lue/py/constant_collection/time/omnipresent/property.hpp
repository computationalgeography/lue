#pragma once
#include "lue/constant_collection/time/omnipresent/property.hpp"
#include <pybind11/pybind11.h>


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {

pybind11::object   cast_to_specialized_property(
                                        Property const& property);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
