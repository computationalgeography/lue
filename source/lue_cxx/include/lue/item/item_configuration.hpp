#pragma once
#include "lue/configuration.hpp"


namespace lue {

using ItemConfiguration = Configuration<
    CollectionVariability,
    ShapeVariability,
    ShapePerItem,
    ValueVariability
>;

}  // namespace lue
