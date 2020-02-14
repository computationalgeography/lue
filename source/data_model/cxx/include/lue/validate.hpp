#pragma once
#include "lue/object/dataset.hpp"
#include "lue/hdf5/file.hpp"
#include "lue/hdf5/issues.hpp"


namespace lue {

std::string        message             (hdf5::Issues const& issues);

void               validate            (Dataset const& dataset,
                                        hdf5::Issues& issues);

void               validate            (hdf5::File const& file,
                                        hdf5::Issues& issues);

void               validate            (std::string const& pathname,
                                        hdf5::Issues& issues);

void               assert_is_valid     (Dataset const& dataset,
                                        bool fail_on_warning=true);

void               assert_is_valid     (hdf5::File const& file,
                                        bool fail_on_warning=true);

void               assert_is_valid     (std::string const& name,
                                        bool fail_on_warning=true);

}  // namespace lue
