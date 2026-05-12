#pragma once
#include "lue/framework/algorithm/atan.hpp"


namespace lue {
    namespace policy::atan {

        template<std::floating_point Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::atan


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            atan, policy::atan::DefaultPolicies, std::floating_point)

    }  // namespace default_policies
}  // namespace lue
