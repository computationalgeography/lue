#pragma once
#include "lue/framework/algorithm/floor.hpp"


namespace lue {
    namespace policy::floor {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::floor


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            floor, policy::floor::DefaultPolicies)

    }  // namespace default_policies
}  // namespace lue
