#pragma once
#include "lue/framework/core/type_traits.hpp"
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
        // TODO!!!
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


/*!
    @brief      Fill a partitioned array in-place with a uniform random
                value from the range [@a min_value, @a max_value]
    @tparam     Element Type of elements in the array
    @tparam     rank Rank of the input array
    @tparam     Array Class template of the type of the array
    @param      array Partitioned array
    @param      min_value Future to minimum value of range from which
                to select values
    @param      max_value Future to maximum value of range from which
                to select values
    @return     Future that becomes ready once the algorithm has finished
    @todo       Implement!

    The existing @a array passed in is updated. Use the returned future if
    you need to know when the filling is done.
*/
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

        futures[p] = hpx::get_colocation_id(partition.get_id()).then(
            hpx::util::unwrapping(
                [=](
                    hpx::id_type const locality_id)
                {
                    return hpx::dataflow(
                        hpx::launch::async,
                        action,
                        locality_id,
                        partition,
                        min_value,
                        max_value);
                }
            )
        );

    }

    return hpx::when_all(futures);
}

}  // namespace lue
