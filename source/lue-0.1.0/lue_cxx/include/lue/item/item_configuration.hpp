#pragma once
#include "lue/configuration.hpp"


namespace lue {

using ItemConfiguration = Configuration<
    ShapePerItem,
    Occurrence,
    ShapeVariability,
    CollectionVariability
>;

}  // namespace lue
