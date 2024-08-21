#pragma once
#include "lue/framework/algorithm/cast.hpp"


namespace lue {
    namespace policy::cast {

        template<typename OutputElement, typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

    }  // namespace policy::cast


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
            cast, policy::cast::DefaultPolicies)

    }  // namespace default_policies
}  // namespace lue
