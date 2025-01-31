#pragma once
#include "lue/data_model.hpp"
// #include <hpx/future.hpp>


namespace lue {

    auto open_dataset(std::string const& pathname, unsigned int flags) -> data_model::Dataset;

}  // namespace lue
