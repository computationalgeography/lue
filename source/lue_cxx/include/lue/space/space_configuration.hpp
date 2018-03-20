#pragma once
#include "lue/configuration.hpp"


namespace lue {

using SpaceConfiguration = Configuration<
    Occurrence,
    CollectionVariability,
    Mobility,
    SpaceDomainItemType
>;

}  // namespace lue
