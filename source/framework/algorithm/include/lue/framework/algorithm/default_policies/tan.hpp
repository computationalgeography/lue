#pragma once
#include "lue/framework/algorithm/tan.hpp"


namespace lue {
    namespace policy::tan {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::tan


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            tan, policy::tan::DefaultPolicies)

    }  // namespace default_policies
}  // namespace lue
