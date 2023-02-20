#pragma once
#include "lue/framework/algorithm/subtract.hpp"


namespace lue {
    namespace policy::subtract {

        template<typename OutputElement, typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::subtract


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS2(subtract, detail::Subtract, policy::subtract::DefaultPolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(-, subtract)

    }  // namespace default_policies

}  // namespace lue
