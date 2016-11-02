#pragma once
#include <hdf5.h>
#include <string>


namespace lue {
namespace hdf5 {

bool               datatypes_are_equal (hid_t const type_id1,
                                        hid_t const type_id2);

std::string        native_datatype_as_string(
                                        hid_t const type_id);

std::string        standard_datatype_as_string(
                                        hid_t const type_id);

hid_t              parse_standard_datatype(
                                        std::string const& string);

bool               is_standard_datatype(hid_t const type_id);

bool               is_native_datatype  (hid_t const type_id);

hid_t              native_type_id      (hid_t const file_type_id);

}  // namespace hdf5
}  // namespace lue
