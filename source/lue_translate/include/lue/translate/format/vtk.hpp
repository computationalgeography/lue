#pragma once
#include "lue/translate/metadata.hpp"
#include "lue/dataset.hpp"


namespace lue {
namespace utility {

void               translate_lue_dataset_to_vtk(
                                        Dataset const& dataset,
                                        std::string const& vtk_filename,
                                        Metadata const& metadata);

}  // namespace utility
}  // namespace lue
