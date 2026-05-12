#pragma once
#include "lue/framework/algorithm/trunc.hpp"


namespace lue {
    namespace policy::trunc {

        template<std::floating_point Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::trunc


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            trunc, policy::trunc::DefaultPolicies, std::floating_point)

    }  // namespace default_policies
}  // namespace lue
