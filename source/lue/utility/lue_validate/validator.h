#pragma once
#include "lue/utility/lue_validate/error.h"

// #include "lue/cxx_api/define.h"
// #include "lue/cxx_api/hdf5.h"
// #include "lue/cxx_api/shape.h"
// #include <vector>
// #include <string>
// #include <tuple>

namespace lue {

class Dataset;

void               validate_dataset    (hdf5::File const& file,
                                        Errors& errors);

}  // namespace lue
