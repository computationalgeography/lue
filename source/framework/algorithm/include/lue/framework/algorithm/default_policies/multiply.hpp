#pragma once
#include "lue/framework/algorithm/multiply.hpp"


namespace lue {
    namespace policy::multiply {

        template<typename Element>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<Element, Element>,
            OutputElements<Element>,
            InputElements<Element, Element>>;

    }  // namespace policy::multiply


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            multiply, policy::multiply::DefaultPolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(*, multiply)

    }  // namespace default_policies

}  // namespace lue
