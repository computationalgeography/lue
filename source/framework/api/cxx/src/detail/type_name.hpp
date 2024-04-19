#pragma once
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array.hpp"
#include <fmt/format.h>
#include <string>
#include <typeinfo>


namespace lue::detail {

    template<typename Element>
    auto type_name() -> std::string
    {
        // TODO Make this name nicer in a portable way
        return typeid(Element).name();
    }


    template<typename Element>
    auto type_name([[maybe_unused]] Element const& argument) -> std::string
    {
        return fmt::format("{}", type_name<Element>());
    }


    template<typename Element>
    auto type_name([[maybe_unused]] Scalar<Element> const& argument) -> std::string
    {
        return fmt::format("Scalar<{}>", type_name<Element>());
    }


    template<typename Element, Rank rank>
    auto type_name([[maybe_unused]] PartitionedArray<Element, rank> const& argument) -> std::string
    {
        return fmt::format("Array<{}, {}>", type_name<Element>(), rank);
    }

}  // namespace lue::detail
