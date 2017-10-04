#pragma once
#include "lue/issues.hpp"
#include "lue/hdf5/file.hpp"


namespace lue {

void               validate            (hdf5::File const& file,
                                        Issues& issues);

void               validate            (std::string const& name,
                                        Issues& issues);

void               assert_is_valid     (hdf5::File const& file,
                                        bool const fail_on_warning=true);

void               assert_is_valid     (std::string const& name,
                                        bool const fail_on_warning=true);

}  // namespace lue
