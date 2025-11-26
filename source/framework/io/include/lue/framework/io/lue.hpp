#pragma once
#include "lue/framework/core/define.hpp"
#include "lue/framework/io/export.hpp"
#include "lue/framework/io/serializer.hpp"
#include <filesystem>


namespace lue::detail {

    using OpenFileSerializer = Serializer<std::filesystem::path, Count>;


    LUE_FRAMEWORK_IO_EXPORT auto open_file_serializer() -> OpenFileSerializer&;

    LUE_FRAMEWORK_IO_EXPORT auto normalize(std::string const& pathname) -> std::filesystem::path;

    LUE_FRAMEWORK_IO_EXPORT auto open_file_promise_for(std::filesystem::path const& path, Count open_count)
        -> hpx::promise<void>;

    LUE_FRAMEWORK_IO_EXPORT auto open_file_when_predecessor_done(
        std::filesystem::path const& path, Count open_count) -> hpx::future<void>;

    LUE_FRAMEWORK_IO_EXPORT auto open_count(std::filesystem::path const& dataset_path) -> Count;

}  // namespace lue::detail
