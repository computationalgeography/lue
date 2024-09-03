#pragma once
#include "lue/framework/algorithm/abs.hpp"


namespace lue {
    namespace policy::abs {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::abs


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            abs, policy::abs::DefaultPolicies)

    }  // namespace default_policies
}  // namespace lue
