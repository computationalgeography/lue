#pragma once
#include <hpx/future.hpp>
#include <fmt/format.h>
#include <string>


namespace lue {

    hpx::future<std::string> system_description();

    void write_debug_message(std::string const& message);

    template<typename Class>
    std::string describe(Class const& instance);


    template<typename Count, typename Idx>
    void validate_idxs(Count const* count, Idx const idx)
    {
        if (idx >= *count)
        {
            throw std::runtime_error(fmt::format("Index out of bounds ({} >= {})", idx, *count));
        }
    }


    template<typename Count, typename Idx, typename... Idxs>
    void validate_idxs(Count* count, Idx const idx, Idxs... idxs)
    {
        if (idx >= *count)
        {
            throw std::runtime_error(fmt::format("Index out of bounds ({} >= {})", idx, *count));
        }

        validate_idxs(++count, idxs...);
    }


    template<typename Shape, typename... Idxs>
    void validate_idxs(Shape const& shape, Idxs... idxs)
    {
        if (sizeof...(idxs) != std::size(shape))
        {
            throw std::runtime_error(fmt::format(
                "Number of indices does not match rank of array ({} != {})",
                sizeof...(idxs),
                std::size(shape)));
        }

        // Verify each index is within the extent of the corresponding
        // array extent
        validate_idxs(shape.data(), idxs...);
    }

}  // namespace lue
