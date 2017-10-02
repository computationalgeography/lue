#pragma once
#include "lue/issues.hpp"
#include "lue/hdf5/file.hpp"


namespace lue {

void               validate            (std::string const& name,
                                        Issues& issues);

}  // namespace lue
