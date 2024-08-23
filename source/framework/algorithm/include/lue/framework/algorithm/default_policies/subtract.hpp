#pragma once
#include "lue/framework/algorithm/subtract.hpp"


namespace lue {
    namespace policy::subtract {

        template<typename Element>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<Element, Element>,
            OutputElements<Element>,
            InputElements<Element, Element>>;

    }  // namespace policy::subtract


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            subtract, policy::subtract::DefaultPolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(-, subtract)

    }  // namespace default_policies

}  // namespace lue
