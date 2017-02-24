#pragma once
#include <ostream>


namespace lue {

class Dataset;

void               describe_dataset    (Dataset const& dataset,
                                        std::ostream& stream);

}  // namespace lue
