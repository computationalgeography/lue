#pragma once
#include <unordered_map>


namespace lue {

    template<typename FromElement, typename ToElement>
    using LookupTable = std::unordered_map<FromElement, ToElement>;

}  // namespace lue
