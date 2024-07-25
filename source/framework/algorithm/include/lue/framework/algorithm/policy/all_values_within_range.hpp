#pragma once
#include "lue/framework/algorithm/policy/policy_traits.hpp"


namespace lue::policy {

    /*!
        @brief      Output range policy that does nothing
        @tparam     OutputElement Type of the output element
        @tparam     InputElement Types of the input elements

        Use this policy if you do not want to check for out-of-rangeness in the output
    */
    template<typename OutputElement, typename... InputElement>
    class AllValuesWithinRange
    {
        public:

            /*!
                @brief      Return whether all @a element values are within the range
                @param      result Result value

                This function always returns `true`.
            */
            static constexpr auto within_range(
                [[maybe_unused]] InputElement const... element, [[maybe_unused]] OutputElement const result)
                -> bool
            {
                return true;
            }
    };


    namespace detail {

        template<typename OutputElement, typename... InputElement>
        class TypeTraits<AllValuesWithinRange<OutputElement, InputElement...>>
        {

            public:

                using Element = OutputElement;

                template<typename E_>
                using Policy = AllValuesWithinRange<E_, InputElement...>;
        };

    }  // namespace detail
}  // namespace lue::policy
