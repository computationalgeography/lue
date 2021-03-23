#pragma once
#include "lue/framework/algorithm/policy/policies.hpp"
#include "lue/framework/algorithm/policy/mark_no_data_by_value.hpp"
#include "lue/framework/algorithm/policy/input_policies.hpp"
#include "lue/framework/algorithm/policy/output_policies.hpp"
#include "lue/framework/algorithm/policy/detect_no_data_by_value.hpp"
#include <hpx/serialization.hpp>
#include <array>


namespace lue {
namespace policy {

template<
    typename... Elements>
using InputElements = detail::TypeList<Elements...>;


template<
    typename... Elements>
using OutputElements = detail::TypeList<Elements...>;


template<
    typename DomainPolicy,
    typename OutputElements,
    typename InputElements>
class DefaultValuePolicies
{
};


/*!
    @brief      Default value policies to use by algorithms

    Defaults:
    - Output no-data policy marks no-data by value
    - Input no-data policy detects value by value
*/
template<
    typename DomainPolicy,
    typename... OutputElements,
    typename... InputElements>
class DefaultValuePolicies<
    DomainPolicy,
    detail::TypeList<OutputElements...>,
    detail::TypeList<InputElements...>>:

    public Policies<
        DomainPolicy,
        OutputsPolicies<OutputPolicies<MarkNoDataByValue<OutputElements>>...>,
        InputsPolicies<InputPolicies<DetectNoDataByValue<InputElements>>...>>

{
};

}  // namespace policy
}  // namespace lue
