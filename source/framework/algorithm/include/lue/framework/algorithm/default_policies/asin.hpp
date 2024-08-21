#pragma once
#include "lue/framework/algorithm/asin.hpp"


namespace lue {
    namespace policy::asin {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::asin


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            asin, policy::asin::DefaultPolicies)

    }  // namespace default_policies
}  // namespace lue
