#pragma once
#include "lue/framework/algorithm/cast.hpp"


namespace lue {

    namespace detail {

        // Primary template which handles case when converting from From to To is a narrowing cast
        template<typename From, typename To, typename = void>
        struct is_narrowing_cast_impl: std::true_type
        {
        };

        // Specialization which handles case when converting from From to To is not a narrowing cast
        template<typename From, typename To>
        struct is_narrowing_cast_impl<From, To, std::void_t<decltype(To{std::declval<From>()})>>:
            std::false_type
        {
        };

    }  // namespace detail


    template<typename From, typename To>
    struct is_narrowing_cast: detail::is_narrowing_cast_impl<From, To>
    {
    };


    template<typename From, typename To>
    constexpr bool is_narrowing_cast_v = is_narrowing_cast<From, To>::value;


    namespace policy::cast {

        template<typename T>
        constexpr bool is_signed_integral_v = std::is_integral_v<T>&& std::is_signed_v<T>;

        template<typename T>
        constexpr bool is_unsigned_integral_v = std::is_integral_v<T>&& std::is_unsigned_v<T>;


        template<typename OutputElement, typename InputElement>
        class RangePolicy
        {

            public:

                static constexpr auto within_range(
                    [[maybe_unused]] InputElement const& argument,
                    [[maybe_unused]] OutputElement const& result) -> bool
                {
                    // The InputElement argument is out of range if it can't be represented by
                    // the OutputElement. This can, for example, happen in case the input type
                    // covers a range of values that is not covered by the output type:
                    // - large signed int -> small signed int
                    // - large unsigned int -> small unsigned int
                    // - signed int -> unsigned int
                    // - large float -> small float
                    // - float -> int / unsigned int

                    if constexpr (
                        is_signed_integral_v<InputElement> && is_signed_integral_v<OutputElement> &&
                        is_narrowing_cast_v<InputElement, OutputElement>)
                    {
                        // large int -> small int
                        return argument >= InputElement{std::numeric_limits<OutputElement>::min()} &&
                               argument <= InputElement{std::numeric_limits<OutputElement>::max()};
                    }

                    if constexpr (
                        is_unsigned_integral_v<InputElement> && is_unsigned_integral_v<OutputElement> &&
                        is_narrowing_cast_v<InputElement, OutputElement>)
                    {
                        // large unsigned int -> small unsigned int
                        return argument <= InputElement{std::numeric_limits<OutputElement>::max()};
                    }

                    if constexpr (is_signed_integral_v<InputElement> && is_unsigned_integral_v<OutputElement>)
                    {
                        // int -> unsigned int
                        if constexpr (sizeof(InputElement) <= sizeof(OutputElement))
                        {
                            return argument >= InputElement{0};
                        }

                        if constexpr (sizeof(InputElement) > sizeof(OutputElement))
                        {
                            return argument >= InputElement{0} &&
                                   argument <=
                                       static_cast<InputElement>(std::numeric_limits<OutputElement>::max());
                        }
                    }

                    if constexpr (
                        is_unsigned_integral_v<InputElement> && is_signed_integral_v<OutputElement> &&
                        sizeof(InputElement) >= sizeof(OutputElement))
                    {
                        // unsigned int → int and large → small
                        // The largest output value must be representable by the input element type
                        return argument <= InputElement{std::numeric_limits<OutputElement>::max()};
                    }

                    if constexpr (
                        std::is_floating_point_v<InputElement> && std::is_floating_point_v<OutputElement> &&
                        is_narrowing_cast_v<InputElement, OutputElement>)
                    {
                        // large float -> small float
                        return argument >= InputElement{std::numeric_limits<OutputElement>::lowest()} &&
                               argument <= InputElement{std::numeric_limits<OutputElement>::max()};
                    }

                    if constexpr (std::is_floating_point_v<InputElement> && std::is_integral_v<OutputElement>)
                    {
                        // float -> int / unsigned int
                        return argument >= InputElement{std::numeric_limits<OutputElement>::min()} &&
                               argument <=
                                   static_cast<InputElement>(std::numeric_limits<OutputElement>::max());
                    }

                    return true;
                }
        };


        template<typename OutputElement, typename InputElement>
        using DefaultValuePolicies = policy::Policies<
            AllValuesWithinDomain<InputElement>,
            OutputsPolicies<OutputPolicies<
                DefaultOutputNoDataPolicy<OutputElement>,
                RangePolicy<OutputElement, InputElement>>>,
            InputsPolicies<InputPolicies<DefaultInputNoDataPolicy<InputElement>>>>;

    }  // namespace policy::cast


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
            cast, policy::cast::DefaultValuePolicies)

    }  // namespace value_policies
}  // namespace lue
