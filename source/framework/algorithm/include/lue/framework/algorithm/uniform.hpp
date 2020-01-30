#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>
#include <algorithm>
#include <random>


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

    static hpx::future<void> uniform_partition(
        Partition const& input_partition,
        ElementT<Partition> const min_value,
        ElementT<Partition> const max_value)
    {
        using Element = ElementT<Partition>;

        assert(
            hpx::get_colocation_id(input_partition.get_id()).get() ==
            hpx::find_here());

        using InputData = DataT<Partition>;

        // Will be used to obtain a seed for the random number engine
        std::random_device random_device;

        // Standard mersenne_twister_engine seeded with the random_device
        std::mt19937 random_number_engine(random_device());

        auto distribution = [min_value, max_value]() {
            if constexpr(std::is_floating_point_v<Element>) {
                return std::uniform_real_distribution<Element>{
                    min_value, max_value};
            }
            else if constexpr(std::is_integral_v<Element>) {
                return std::uniform_int_distribution<Element>{
                    min_value, max_value};
            }
        }();

        auto const input_partition_server_ptr{
            hpx::get_ptr(input_partition).get()};
        auto const& input_partition_server{*input_partition_server_ptr};

        InputData input_partition_data{
            input_partition_server.data(CopyMode::share)};
        TargetIndex const target_idx = input_partition_data.target_idx();

        return hpx::async(
            numa_domain_executor(target_idx),
            hpx::util::annotated_function(

                [
                    input_partition_data{std::move(input_partition_data)},
                    distribution{std::move(distribution)},
                    random_number_engine{std::move(random_number_engine)}
                ] () mutable
                {
                    // Fill the data collection passed in
                    std::generate(
                            input_partition_data.begin(),
                            input_partition_data.end(),

                            [&]()
                            {
                                return distribution(random_number_engine);
                            }

                        );
                },

                "uniform_partition"));

        // return hpx::dataflow(
        //     numa_domain_executor(target_idx),
        //     hpx::util::annotated_function(
        //         hpx::util::unwrapping(

        //             [
        //                 distribution{std::move(distribution)},
        //                 random_number_engine{std::move(random_number_engine)}
        //             ] (
        //                 InputData&& input_partition_data) mutable
        //             {
        //                 // Fill the data collection passed in
        //                 std::generate(
        //                         input_partition_data.begin(), input_partition_data.end(),

        //                         [&]()
        //                         {
        //                             return distribution(random_number_engine);
        //                         }

        //                     );
        //             }

        //         ),
        //         "uniform_partition"),

        //     input_partition.data(CopyMode::share));
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&uniform_partition),
            &uniform_partition,
            Action>
    {};

};

}  // namespace uniform
}  // namespace detail


template<
    typename T>
using UniformAction =
    typename detail::uniform::OverloadPicker<T>::Action;


/*!
    @brief      Fill a partitioned array in-place with a uniform random
                value from the range @a min_value - @a max_value
    @tparam     Element Type of elements in the array
    @tparam     rank Rank of the input array
    @tparam     Array Class template of the type of the array
    @param      array Partitioned array
    @param      min_value Future to minimum value of range from which
                to select values
    @param      max_value Future to maximum value of range from which
                to select values
    @return     Future that becomes ready once the algorithm has finished

    The existing @a array passed in is updated. Use the returned future if
    you need to know when the filling is done.

    If @a Element is a floating point value, then the generated values
    will be from the half open interval [@a min_value, @a max_value).
    To fill the array with values from the closed interval [min_value,
    max_value], pass std::nextafter(max_value,
    std::numeric_limits<Element>::max()) as the second parameter.

    If @a Element is an integral value, then the generated values will
    be from the closed interval [@a min_value, @a max_value].
*/
template<
    typename Element,
    Rank rank,
    template<typename, Rank> typename Array>
[[nodiscard]] hpx::future<void> uniform(
    Array<Element, rank>& array,
    hpx::shared_future<Element> const& min_value,
    hpx::shared_future<Element> const& max_value)
{
    using Partition = PartitionT<Array<Element, rank>>;

    UniformAction<Partition> action;

    std::vector<hpx::future<void>> futures(nr_partitions(array));

    for(Index p = 0; p < nr_partitions(array); ++p) {

        futures[p] = hpx::dataflow(
            hpx::launch::async,

            [action](
                Partition const& input_partition,
                hpx::shared_future<Element> const& min_value,
                hpx::shared_future<Element> const& max_value,
                hpx::future<hpx::id_type>&& locality_id)
            {
                return action(
                    locality_id.get(),
                    input_partition,
                    min_value.get(), max_value.get());
            },

            array.partitions()[p],
            min_value,
            max_value,
            hpx::get_colocation_id(array.partitions()[p].get_id()));

    }

    return hpx::when_all(std::move(futures));
}

}  // namespace lue
