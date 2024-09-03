#pragma once
#include "lue/framework/algorithm/round.hpp"


namespace lue {
    namespace policy::round {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::round


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            round, policy::round::DefaultPolicies)

    }  // namespace default_policies
}  // namespace lue
