#pragma once
// #include "lue/framework/algorithm/policy/detail/type_list.hpp"
#include "lue/framework/algorithm/policy/policies.hpp"
#include "lue/framework/algorithm/policy/dont_mark_no_data.hpp"
#include "lue/framework/algorithm/policy/fill_halo_with_constant_value.hpp"
#include "lue/framework/algorithm/policy/input_policies.hpp"
#include "lue/framework/algorithm/policy/output_policies.hpp"
#include "lue/framework/algorithm/policy/skip_no_data.hpp"
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
class DefaultPolicies
{
};


/*!
    @brief      Default policies to use by algorithms

    Defaults:
    - Output no-data policy does not mark no-data in the output
    - Input no-data policy does not check for no-data in the input
*/
template<
    typename DomainPolicy,
    typename... OutputElements,
    typename... InputElements>
class DefaultPolicies<
    DomainPolicy,
    detail::TypeList<OutputElements...>,
    detail::TypeList<InputElements...>>:

    public Policies<
        DomainPolicy,
        OutputsPolicies<OutputPolicies<DontMarkNoData<OutputElements>>...>,
        InputsPolicies<InputPolicies<SkipNoData<InputElements>>...>>

{
};


template<
    typename DomainPolicy,
    typename OutputElements,
    typename InputElements>
class DefaultFocalOperationPolicies
{
};


/*!
    @brief      Default policies to use by focal operations

    Defaults:
    - Outputs:
       - No-data policy does not mark no-data in the output
    - Inputs:
       - No-data policy does not check for no-data in the input
       - Halos are filled with a constant value
*/
template<
    typename DomainPolicy,
    typename... OutputElements,
    typename... InputElements>
class DefaultFocalOperationPolicies<
        DomainPolicy,
        detail::TypeList<OutputElements...>,
        detail::TypeList<InputElements...>>:

    public Policies<
        DomainPolicy,
        OutputsPolicies<OutputPolicies<DontMarkNoData<OutputElements>>...>,
        InputsPolicies<
            FocalOperationInputPolicies<
                SkipNoData<InputElements>, FillHaloWithConstantValue<InputElements>>...>>

{

    private:

        using Base = Policies<
            DomainPolicy,
            OutputsPolicies<
                OutputPolicies<
                    DontMarkNoData<OutputElements>>...>,
            InputsPolicies<
                FocalOperationInputPolicies<
                    SkipNoData<InputElements>,
                    FillHaloWithConstantValue<InputElements>>...>>;

    public:

        DefaultFocalOperationPolicies(
            InputElements const... fill_values):

            Base{
                    DomainPolicy{},
                    OutputPolicies<
                            DontMarkNoData<OutputElements>
                        >{}...,
                    FocalOperationInputPolicies<
                            SkipNoData<InputElements>,
                            FillHaloWithConstantValue<InputElements>
                        >{FillHaloWithConstantValue<InputElements>{fill_values}}...
                }

        {
        }

};

}  // namespace policy
}  // namespace lue
