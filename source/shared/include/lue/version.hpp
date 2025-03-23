#pragma once
#include <cstdint>
#include <string>


namespace lue {

    auto version() -> std::string;

    auto major_version() -> std::uint32_t;

    auto minor_version() -> std::uint32_t;

    auto patch_version() -> std::uint32_t;

    auto tweak_version() -> std::uint32_t;

}  // namespace lue
