#pragma once
#include "lue/framework/algorithm/close_to.hpp"


namespace lue {
    namespace policy::close_to {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::close_to


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS2(
            close_to, detail::CloseTo, policy::close_to::DefaultPolicies)

    }  // namespace default_policies

}  // namespace lue
