#pragma once
#include "lue/framework/core/define.hpp"
#include "lue/framework/io/export.hpp"
#include "lue/framework/io/serializer.hpp"
#include "lue/configure.hpp"
#include "lue/hdf5/configure.hpp"
#include <filesystem>


namespace lue::detail {

    template<bool condition, typename A, typename B>
    constexpr auto ternary_if(A&& a, B&& b)
    {
        if constexpr (condition)
        {
            return std::forward<A>(a);
        }
        else
        {
            return std::forward<B>(b);
        }
    }


    // enum class IOStrategy : std::uint8_t {
    //     SerialThreadSafe,
    //     SerialNonThreadSafe,
    //     Parallel,
    // };
    //
    //
    // static constexpr IOStrategy io_strategy =


    // E.g.: cluster with parallel filesystem
    // This implies hdf5::BuildOptions::hdf5_is_parallel is true
    static constexpr bool parallel_io = BuildOptions::framework_with_parallel_io;

    static constexpr bool serial_io = !parallel_io;

    // E.g.: Ubuntu Linux, custom HDF5 builds
    static constexpr bool serial_io_thread_safe = serial_io && hdf5::BuildOptions::hdf5_is_threadsafe;

    // E.g.: most default HDF5 installations
    static constexpr bool serial_io_non_thread_safe = serial_io && (!hdf5::BuildOptions::hdf5_is_threadsafe);


    LUE_FRAMEWORK_IO_EXPORT auto normalize(std::string const& pathname) -> std::filesystem::path;


    LUE_FRAMEWORK_IO_EXPORT auto from_lue_order(std::filesystem::path const& dataset_path) -> Count;

    LUE_FRAMEWORK_IO_EXPORT auto to_lue_order(std::filesystem::path const& dataset_path) -> Count;


    LUE_FRAMEWORK_IO_EXPORT auto current_from_lue_order(std::filesystem::path const& dataset_path) -> Count;

    LUE_FRAMEWORK_IO_EXPORT auto current_to_lue_order(std::filesystem::path const& dataset_path) -> Count;


#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO

    LUE_FRAMEWORK_IO_EXPORT void add_to_lue_finished(
        std::filesystem::path const& path, Count count, hpx::shared_future<void> future);

    LUE_FRAMEWORK_IO_EXPORT void add_from_lue_finished(
        std::filesystem::path const& path, Count count, hpx::shared_future<void> future);

    LUE_FRAMEWORK_IO_EXPORT auto to_lue_finished(std::filesystem::path const& path, Count count)
        -> hpx::shared_future<void>;

    LUE_FRAMEWORK_IO_EXPORT auto from_lue_finished(std::filesystem::path const& path, Count count)
        -> hpx::shared_future<void>;
#endif


    LUE_FRAMEWORK_IO_EXPORT auto from_lue_open_dataset_promise_for(
        std::filesystem::path const& path, Count open_count) -> hpx::promise<void>;

    LUE_FRAMEWORK_IO_EXPORT auto from_lue_close_dataset_promise_for(
        std::filesystem::path const& path, Count open_count) -> hpx::promise<void>;

    LUE_FRAMEWORK_IO_EXPORT auto to_lue_open_dataset_promise_for(
        std::filesystem::path const& path, Count open_count) -> hpx::promise<void>;

    LUE_FRAMEWORK_IO_EXPORT auto to_lue_close_dataset_promise_for(
        std::filesystem::path const& path, Count open_count) -> hpx::promise<void>;


    LUE_FRAMEWORK_IO_EXPORT auto from_lue_open_dataset_when_predecessor_done(
        std::filesystem::path const& path, Count open_count) -> hpx::shared_future<void>;

    LUE_FRAMEWORK_IO_EXPORT auto from_lue_close_dataset_when_predecessor_done(
        std::filesystem::path const& path, Count open_count) -> hpx::shared_future<void>;

    LUE_FRAMEWORK_IO_EXPORT auto to_lue_open_dataset_when_predecessor_done(
        std::filesystem::path const& path, Count open_count) -> hpx::shared_future<void>;

    LUE_FRAMEWORK_IO_EXPORT auto to_lue_close_dataset_when_predecessor_done(
        std::filesystem::path const& path, Count open_count) -> hpx::shared_future<void>;


    LUE_FRAMEWORK_IO_EXPORT auto from_lue_close_dataset_done_available(
        std::filesystem::path const& path, Count count) -> bool;

    LUE_FRAMEWORK_IO_EXPORT auto from_lue_close_dataset_done(std::filesystem::path const& path, Count count)
        -> hpx::shared_future<void>;

    LUE_FRAMEWORK_IO_EXPORT auto to_lue_close_dataset_done_available(
        std::filesystem::path const& path, Count count) -> bool;

    LUE_FRAMEWORK_IO_EXPORT auto to_lue_close_dataset_done(std::filesystem::path const& path, Count count)
        -> hpx::shared_future<void>;

}  // namespace lue::detail
