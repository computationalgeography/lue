#pragma once
#include "lue/framework/algorithm/policy/policy_traits.hpp"


namespace lue::policy {

    template<typename OutputElement, typename... InputElement>
    class AllValuesWithinRange
    {
        public:

            constexpr static bool within_range(
                [[maybe_unused]] InputElement const... argument, [[maybe_unused]] OutputElement const result)
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
