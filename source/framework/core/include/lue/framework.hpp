#pragma once
#include "lue/framework/configure.hpp"
#include <algorithm>


namespace lue {

    constexpr auto rank_supported(Rank const rank) -> bool
    {
        return std::find(ranks.begin(), ranks.end(), rank) != ranks.end();
    }


    void verify_rank_supported(Rank rank);


    constexpr static std::size_t nr_unsigned_integral_elements = std::tuple_size_v<UnsignedIntegralElements>;

    constexpr static std::size_t nr_signed_integral_elements = std::tuple_size_v<SignedIntegralElements>;

    constexpr static std::size_t nr_floating_point_elements = std::tuple_size_v<FloatingPointElements>;


    template<std::size_t idx>
    using UnsignedIntegralElement = std::tuple_element_t<idx, UnsignedIntegralElements>;

    template<std::size_t idx>
    using SignedIntegralElement = std::tuple_element_t<idx, SignedIntegralElements>;

    template<std::size_t idx>
    using FloatingPointElement = std::tuple_element_t<idx, FloatingPointElements>;


    namespace detail {

        template<typename... B>
        using or_ = std::disjunction<B...>;

        template<typename T, typename Tuple>
        struct has_type;

        template<typename T, typename... Us>
        struct has_type<T, std::tuple<Us...>>: or_<std::is_same<T, Us>...>
        {
        };

        template<typename Element, typename Elements>
        using element_type_supported = has_type<Element, Elements>;

        template<typename Element, typename Elements>
        constexpr bool element_type_supported_v = element_type_supported<Element, Elements>::value;

    }  // namespace detail


    template<typename Element>
    constexpr bool unsigned_integral_element_supported =
        detail::element_type_supported_v<Element, UnsignedIntegralElements>;


    template<typename Element>
    constexpr bool signed_integral_element_supported =
        detail::element_type_supported_v<Element, SignedIntegralElements>;


    template<typename Element>
    constexpr bool integral_element_supported = detail::element_type_supported_v<Element, IntegralElements>;


    template<typename Element>
    constexpr bool floating_point_element_supported =
        detail::element_type_supported_v<Element, FloatingPointElements>;


    template<typename Element>
    constexpr bool arithmetic_element_supported =
        detail::element_type_supported_v<Element, ArithmeticElements>;


    using LargestIntegralElement = std::conditional_t<
        signed_integral_element_supported<std::int64_t>,
        std::int64_t,
        std::conditional_t<
            unsigned_integral_element_supported<std::uint64_t>,
            std::uint64_t,
            std::conditional_t<
                signed_integral_element_supported<std::int32_t>,
                std::int32_t,
                std::conditional_t<
                    unsigned_integral_element_supported<std::uint32_t>,
                    std::uint32_t,
                    std::conditional_t<
                        signed_integral_element_supported<std::int8_t>,
                        std::int8_t,
                        std::conditional_t<
                            unsigned_integral_element_supported<std::uint8_t>,
                            std::uint8_t,
                            void>>>>>>;

    using LargestSignedIntegralElement = std::conditional_t<
        signed_integral_element_supported<std::int64_t>,
        std::int64_t,
        std::conditional_t<
            signed_integral_element_supported<std::int32_t>,
            std::int32_t,
            std::conditional_t<signed_integral_element_supported<std::int8_t>, std::int8_t, void>>>;

    using SmallestSignedIntegralElement = std::conditional_t<
        signed_integral_element_supported<std::int8_t>,
        std::int8_t,
        std::conditional_t<
            signed_integral_element_supported<std::int32_t>,
            std::int32_t,
            std::conditional_t<signed_integral_element_supported<std::int64_t>, std::int64_t, void>>>;

    using LargestUnsignedIntegralElement = std::conditional_t<
        unsigned_integral_element_supported<std::uint64_t>,
        std::uint64_t,
        std::conditional_t<
            unsigned_integral_element_supported<std::uint32_t>,
            std::uint32_t,
            std::conditional_t<unsigned_integral_element_supported<std::uint8_t>, std::uint8_t, void>>>;

    using SmallestUnsignedIntegralElement = std::conditional_t<
        unsigned_integral_element_supported<std::uint8_t>,
        std::uint8_t,
        std::conditional_t<
            unsigned_integral_element_supported<std::uint32_t>,
            std::uint32_t,
            std::conditional_t<unsigned_integral_element_supported<std::uint64_t>, std::uint64_t, void>>>;

    using LargestFloatingPointElement = std::conditional_t<
        floating_point_element_supported<double>,
        double,
        std::conditional_t<floating_point_element_supported<float>, float, void>>;

    using SmallestFloatingPointElement = std::conditional_t<
        floating_point_element_supported<float>,
        float,
        std::conditional_t<floating_point_element_supported<double>, double, void>>;

    static_assert(
        integral_element_supported<BooleanElement>,
        "Reconfigure: BooleanElement must be one of the integral elements");

    static_assert(
        integral_element_supported<CountElement>,
        "Reconfigure: CountElement must be one of the integral elements");

    static_assert(
        integral_element_supported<IndexElement>,
        "Reconfigure: IndexElement must be one of the integral elements");

    static_assert(
        integral_element_supported<FlowDirectionElement>,
        "Reconfigure: FlowDirectionElement must be one of the integral elements");


    template<typename T>
    concept ArithmeticElement = arithmetic_element_supported<T>;

}  // namespace lue
