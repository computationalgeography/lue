#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include <hpx/serialization/serialize.hpp>
#include <cmath>


namespace lue {
namespace detail {

// https://docs.scipy.org/doc/numpy/reference/generated/numpy.isclose.html
template<
    typename OutputElement_,
    typename InputElement>
class CloseTo
{

public:

    static_assert(std::is_floating_point_v<InputElement>);

    using OutputElement = OutputElement_;

    CloseTo(
        InputElement const relative_difference=1e-5,
        InputElement const absolute_difference=1e-8):

        _relative_difference{relative_difference},
        _absolute_difference{absolute_difference}

    {
    }

    constexpr OutputElement operator()(
        InputElement const& input_element1,
        InputElement const& input_element2) const noexcept
    {
        return
            std::abs(input_element1 - input_element2) <=
                (_absolute_difference + _relative_difference * std::abs(input_element2));
    }

    friend class hpx::serialization::access;

    template<
        typename Archive>
    void serialize(
        Archive& archive,
        unsigned int const /* version */)
    {
        archive & _relative_difference & _absolute_difference;
    }

private:

    InputElement _relative_difference;

    InputElement _absolute_difference;

};

}  // namespace detail


namespace policy {
namespace close_to {

using DefaultPolicies = policy::DefaultPolicies<2, 1>;

}  // namespace policy
}  // namespace close_to


template<
    typename OutputElement=bool,
    typename InputElement,
    Rank rank>
PartitionedArray<OutputElement, rank> close_to(
    PartitionedArray<InputElement, rank> const& array1,
    PartitionedArray<InputElement, rank> const& array2)
{
    return binary_local_operation(
        policy::close_to::DefaultPolicies{},
        array1, array2,
        detail::CloseTo<OutputElement, InputElement>{});
}


template<
    typename OutputElement=bool,
    typename InputElement,
    Rank rank>
PartitionedArray<OutputElement, rank> close_to(
    PartitionedArray<InputElement, rank> const& array,
    hpx::shared_future<InputElement> const& scalar)
{
    return binary_local_operation(
        policy::close_to::DefaultPolicies{},
        array, scalar,
        detail::CloseTo<OutputElement, InputElement>{});
}


template<
    typename OutputElement=bool,
    typename InputElement,
    Rank rank>
PartitionedArray<OutputElement, rank> close_to(
    hpx::shared_future<InputElement> const& scalar,
    PartitionedArray<InputElement, rank> const& array)
{
    return binary_local_operation(
        policy::close_to::DefaultPolicies{},
        scalar, array,
        detail::CloseTo<OutputElement, InputElement>{});
}


template<
    typename OutputElement=bool,
    typename InputElement,
    Rank rank>
PartitionedArray<OutputElement, rank> close_to(
    PartitionedArray<InputElement, rank> const& array,
    InputElement const scalar)
{
    return close_to<OutputElement>(array, hpx::make_ready_future<InputElement>(scalar).share());
}


template<
    typename OutputElement=bool,
    typename InputElement,
    Rank rank>
PartitionedArray<OutputElement, rank> close_to(
    InputElement const scalar,
    PartitionedArray<InputElement, rank> const& array)
{
    return close_to<OutputElement>(hpx::make_ready_future<InputElement>(scalar).share(), array);
}

}  // namespace lue
