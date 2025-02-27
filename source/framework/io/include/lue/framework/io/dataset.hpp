#pragma once
#include "lue/framework/io/export.hpp"
#include "lue/data_model.hpp"


namespace lue {

    LUE_FRAMEWORK_IO_EXPORT auto open_dataset(std::string const& pathname, unsigned int flags)
        -> data_model::Dataset;

}  // namespace lue
