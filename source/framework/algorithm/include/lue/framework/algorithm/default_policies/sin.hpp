#pragma once
#include "lue/framework/algorithm/sin.hpp"


namespace lue {
    namespace policy::sin {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::sin


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            sin, policy::sin::DefaultPolicies)

    }  // namespace default_policies
}  // namespace lue
