#define BOOST_TEST_MODULE lue framework algorithm create_partitioned_array
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

using Element = std::int64_t;
std::size_t const rank = 2;

using Array = lue::PartitionedArray<Element, rank>;
using Shape = lue::ShapeT<Array>;

Shape const array_shape{{100, 100}};
Shape const partition_shape{{10, 10}};


template<
    typename Element,
    lue::Rank rank>
class NumberPartitionsIndividually
{

    public:

        using OutputElement = Element;
        using Partition = lue::ArrayPartition<OutputElement, rank>;
        using Offset = lue::OffsetT<Partition>;
        using Shape = lue::ShapeT<Partition>;

        static constexpr bool instantiate_per_locality{false};

        Partition instantiate(
            hpx::id_type const locality_id,
            Offset const& offset,
            Shape const& partition_shape)
        {
            return hpx::async(

                    [locality_id, offset, partition_shape]()
                    {
                        return Partition{locality_id, offset, partition_shape};
                    }

                );
        }

    private:

};


template<
    // typename Policies,
    typename Partition>
std::vector<Partition> number_partition_per_locality(
    // Policies const& /* policies */,
    std::vector<lue::OffsetT<Partition>> const& offsets,
    lue::ShapeT<Partition> const& partition_shape)
{
    using Element = lue::ElementT<Partition>;

    std::size_t const nr_partitions{std::size(offsets)};
    std::vector<Partition> partitions(nr_partitions);

    // Iterate over each offset and instantiate partition, passing in
    // the locality id as the fill value.

    hpx::id_type const locality_id{hpx::find_here()};
    Element const fill_value{hpx::get_locality_id()};

    partitions[0] = hpx::async(

            [locality_id, offset=offsets[0], partition_shape, fill_value]()
            {
                return Partition{locality_id, offset, partition_shape, fill_value};
            }

        );

    for(std::size_t idx = 1; idx < nr_partitions; ++idx)
    {
        partitions[idx] = partitions[idx-1].then(

                [locality_id, offset=offsets[idx], partition_shape, fill_value](
                    auto const& /* previous_partition */)
                {
                    return Partition{locality_id, offset, partition_shape, fill_value};
                }

            );
    }

    return partitions;
}


template<
    // typename Policies,
    typename Partition>
struct NumberPartitionsPerLocalityAction:
    hpx::actions::make_action<
            decltype(&number_partition_per_locality</* Policies, */ Partition>),
            &number_partition_per_locality</* Policies, */ Partition>,
            NumberPartitionsPerLocalityAction</* Policies, */ Partition>
        >
{};


template<
    typename Element,
    lue::Rank rank>
class NumberPartitionsPerLocality
{

    public:

        using OutputElement = Element;
        using Partition = lue::ArrayPartition<OutputElement, rank>;
        using Offset = lue::OffsetT<Partition>;
        using Shape = lue::ShapeT<Partition>;

        static constexpr bool instantiate_per_locality{true};

        hpx::future<std::vector<Partition>> instantiate(
            hpx::id_type const locality_id,
            std::vector<Offset> offsets,
            Shape const& partition_shape)
        {
            // Spawn a task that will create all partitions for. It must
            // immediately return a collection of partitions which we
            // can assign to the correct place in the array.

            using Action = NumberPartitionsPerLocalityAction</* Policies, */ Partition>;
            Action action{};

            return hpx::async(

                    [locality_id, action=std::move(action), offsets=std::move(offsets), partition_shape]()
                    {
                        return action(locality_id, offsets, partition_shape);
                    }

                );
        }

    private:

};

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(instantiate_partitions_individually)
{
    // Create a partitioned array in which the elements of the partitions
    // have a unique value per partition. Each partition contains
    // different numbers. The elements within each partition have the
    // same number.

    using Functor = NumberPartitionsIndividually<Element, rank>;

    Array array = lue::create_partitioned_array(array_shape, partition_shape, Functor{});

    // TODO Test results...
    // BOOST_CHECK(false);

    // The number of unique values per partition is 1
    // TODO Depends on availability of partition_count_unique algorithm, gh300
    // Per partition a single count >= 0
    // PartitionedArray<Count, rank> counts = partition_count_unique(array);
    // BOOST_CHECK(lue::all(lue::equal(counts, 1)).get());

    // The number of unique values in the array equals the number of partitions
    // PartitionedArray<Count, 1> unique_values = lue::unique(array);
    // BOOST_CHECK(lue::nr_elements(unique_values) == lue::nr_partitions(array));
}


// BOOST_AUTO_TEST_CASE(instantiate_partitions_per_locality)
// {
//     // Create a partitioned array in which the elements of the partitions
//     // have a unique value per partition per locality. The partitions
//     // instantiated within the same locality contain the same numbers. The
//     // elements within each partition have the same number.
// 
//     using Functor = NumberPartitionsPerLocality<Element, rank>;
// 
//     Array array = lue::create_partitioned_array(array_shape, partition_shape, Functor{});
// 
//     // TODO Test results...
//     BOOST_CHECK(false);
// 
//     // The number of unique values per partition is 1
//     // PartitionedArray<Count, rank> counts = partition_count_unique(array);
//     // future<Count> sum = lue::sum(counts);
// 
//     // The number of unique values in the array equals the number of localities
//     // PartitionedArray<Count, 1> unique_values = lue::unique(array);
//     // BOOST_CHECK(lue::nr_elements(unique_values) == hpx::nr_localities());
// }


// TODO
// BOOST_AUTO_TEST_CASE(clamp_mode_merge)
// {
//     BOOST_CHECK(false);
// }
// 
// 
// BOOST_AUTO_TEST_CASE(clamp_mode_shrink)
// {
//     BOOST_CHECK(false);
// }
