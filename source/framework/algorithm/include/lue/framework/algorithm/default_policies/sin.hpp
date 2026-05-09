#pragma once
#include "lue/framework/algorithm/sin.hpp"


namespace lue {
    namespace policy::sin {

        template<std::floating_point Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::sin


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            sin, policy::sin::DefaultPolicies, std::floating_point)

    }  // namespace default_policies
}  // namespace lue
