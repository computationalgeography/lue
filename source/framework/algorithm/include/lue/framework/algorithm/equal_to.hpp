#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement_=bool>
class EqualTo
{

public:

    using OutputElement = OutputElement_;

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        return input_element1 == input_element2;
    }

};

}  // namespace detail


namespace policy {
namespace equal_to {

using DefaultPolicies = policy::DefaultPolicies<2, 1>;

}  // namespace equal_to
}  // namespace policy


LUE_BINARY_LOCAL_OPERATION_OVERLOADS(equal_to, detail::EqualTo)


// partition == scalar
template<
    typename InputElement,
    Rank rank>
// ArrayPartition<bool, rank> equal_to(
auto equal_to(
    hpx::id_type const locality_id,
    ArrayPartition<InputElement, rank> const& partition,
    hpx::shared_future<InputElement> const& scalar)
{
    using Policies = policy::equal_to::DefaultPolicies;
    using OutputElement = OutputElementT<detail::EqualTo<InputElement>>;

    return binary_local_operation(
        locality_id,
        Policies{},
        partition, scalar,
        detail::EqualTo<InputElement, OutputElement>{});
}


// partition == scalar
template<
    typename InputElement,
    Rank rank>
ArrayPartition<bool, rank> equal_to(
    hpx::id_type const locality_id,
    ArrayPartition<InputElement, rank> const& partition,
    InputElement const& scalar)
{
    return equal_to(locality_id, partition, hpx::make_ready_future<InputElement>(scalar).share());
}


LUE_BINARY_COMPARISON_OPERATOR(==, equal_to)

}  // namespace lue
