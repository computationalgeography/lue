#pragma once
#include <cstdint>


namespace lue::detail {

    enum class AccumulationExitCellClass : std::uint8_t {
        sink,
        partition_output,
        junction,
    };

}  // namespace lue::detail
