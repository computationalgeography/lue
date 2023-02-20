#pragma once
#include <array>


namespace lue {

    template<typename Index, Index rank>
    using Offset = std::array<Index, rank>;

}  // namespace lue
