#pragma once
#include <cstdint>
#include <string>


namespace lue {

    std::string version();

    std::uint32_t major_version();

    std::uint32_t minor_version();

    std::uint32_t patch_version();

    std::uint32_t tweak_version();

}  // namespace lue
