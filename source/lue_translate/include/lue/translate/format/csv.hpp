#pragma once
#include "lue/utility/metadata.hpp"
#include "lue/dataset.hpp"


namespace lue {
namespace utility {

void               translate_lue_dataset_to_csv(
                                        Dataset const& dataset,
                                        std::string const& csv_filename,
                                        Metadata const& metadata);

}  // namespace utility
}  // namespace lue
