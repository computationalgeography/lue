#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {
namespace uniform {

template<
    typename T,
    typename Enable=void>
class OverloadPicker
{
};


template<
    typename Partition>
class OverloadPicker<
    Partition,
    typename std::enable_if_t<is_array_partition_v<Partition>>>

{

public:

    static void uniform_real_partition(
        Partition const& /* partition */,
        hpx::shared_future<ElementT<Partition>> const /* min_value */,
        hpx::shared_future<ElementT<Partition>> const /* max_value */)
    {
        // partition.fill(5);
    }

    struct RealAction:
        hpx::actions::make_action<
            decltype(&uniform_real_partition),
            &uniform_real_partition,
            RealAction>
    {};

};

}  // namespace uniform
}  // namespace detail


template<
    typename T>
using UniformIntAction =
    typename detail::uniform::OverloadPicker<T>::IntAction;


template<
    typename T>
using UniformRealAction =
    typename detail::uniform::OverloadPicker<T>::RealAction;


template<
    typename Element,
    std::size_t rank,
    template<typename, std::size_t> typename Array>
[[nodiscard]] hpx::future<void> uniform_real(
    Array<Element, rank>& array,
    hpx::shared_future<Element> const& min_value,
    hpx::shared_future<Element> const& max_value)
{
    static_assert(std::is_floating_point_v<Element>);

    using Partition = PartitionT<Array<Element, rank>>;

    UniformRealAction<Partition> action;
    std::vector<hpx::future<void>> futures(nr_partitions(array));

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        Partition& partition = array.partitions()[p];

        futures[p] = hpx::dataflow(
            hpx::launch::async,
            action,
            hpx::get_colocation_id(
                hpx::launch::sync, partition.get_id()),
            partition,
            min_value,
            max_value);

    }

    return hpx::when_all(futures);
}

}  // namespace lue
