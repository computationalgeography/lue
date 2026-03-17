#pragma once
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include <format>
#include <string>
#include <type_traits>
#include <typeinfo>


namespace lue::api::detail {

    template<typename Element>
        requires(std::is_arithmetic_v<Element>)
    constexpr auto type_name() -> char const*
    {
        if constexpr (std::is_same_v<Element, std::uint8_t>)
        {
            return "uint8";
        }
        else if (std::is_same_v<Element, std::int32_t>)
        {
            return "int32";
        }
        else if (std::is_same_v<Element, std::uint32_t>)
        {
            return "uint32";
        }
        else if (std::is_same_v<Element, std::int64_t>)
        {
            return "int64";
        }
        else if (std::is_same_v<Element, std::uint64_t>)
        {
            return "uint64";
        }
        else if (std::is_same_v<Element, float>)
        {
            return "float32";
        }
        else if (std::is_same_v<Element, double>)
        {
            return "float64";
        }
        // else if (std::is_same_v<Element, char*> || std::is_same_v<Element, std::string>)
        // {
        //     return "string";
        // }
        else
        {
            return typeid(Element).name();
        }
    }


    template<typename Element>
    auto type_name([[maybe_unused]] Element const& argument) -> std::string
    {
        return std::format("{}", type_name<Element>());
    }


    inline auto type_name([[maybe_unused]] std::string const& string) -> std::string
    {
        return "std::string";
    }


    template<typename Element, std::size_t size>
    auto type_name([[maybe_unused]] std::array<Element, size> const& array) -> std::string
    {
        return std::format("std::array<{}, {}>", type_name<Element>(), size);
    }


    template<typename Element>
    auto type_name([[maybe_unused]] lue::Scalar<Element> const& argument) -> std::string
    {
        return std::format("Scalar<{}>", type_name<Element>());
    }


    template<typename Element, Rank rank>
    auto type_name([[maybe_unused]] lue::Kernel<Element, rank> const& argument) -> std::string
    {
        return std::format("Kernel<{}, {}>", type_name<Element>(), rank);
    }


    template<typename Element, Rank rank>
    auto type_name([[maybe_unused]] PartitionedArray<Element, rank> const& argument) -> std::string
    {
        return std::format("Array<{}, {}>", type_name<Element>(), rank);
    }

}  // namespace lue::api::detail
