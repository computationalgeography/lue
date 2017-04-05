#pragma once
#include "lue/dataset.h"
#include <ostream>
#include <string>


namespace lue {

using DatasetRef = std::reference_wrapper<Dataset const>;
using DatasetRefs = std::vector<DatasetRef>;


void               translate_lue_dataset_to_dot(
                                        DatasetRefs const& datasets,
                                        std::string const& dot_filename);

void               translate_lue_dataset_to_dot(
                                        Dataset const& dataset,
                                        std::ostream& stream);

void               translate_lue_dataset_to_dot(
                                        Dataset const& dataset,
                                        std::string const& dot_filename);

} // namespace lue
