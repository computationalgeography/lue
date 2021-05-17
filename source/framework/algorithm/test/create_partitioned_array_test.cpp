#define BOOST_TEST_MODULE lue framework algorithm create_partitioned_array
#include "lue/framework/algorithm/all.hpp"
#include "lue/framework/algorithm/definition/equal_to.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/partition_count_unique.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/unique.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/core/component.hpp"


namespace {

    using Element = std::int64_t;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;
    using Offset = lue::OffsetT<Array>;

    Shape const array_shape{{60, 40}};
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


            template<
                typename Policies>
            Partition instantiate(
                hpx::id_type const locality_id,
                [[maybe_unused]] Policies const& policies,
                lue::Index const partition_idx,
                Offset const& offset,
                Shape const& partition_shape)
            {
                Element const fill_value{partition_idx};

                return hpx::async(

                        [locality_id, offset, partition_shape, fill_value]()
                        {
                            return Partition{locality_id, offset, partition_shape, fill_value};
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
        std::vector<lue::ShapeT<Partition>> const& partition_shapes)
    {
        using Element = lue::ElementT<Partition>;

        std::size_t const nr_partitions{std::size(offsets)};
        std::vector<Partition> partitions(nr_partitions);

        // Iterate over each offset and instantiate partition, passing in
        // the locality id as the fill value.

        hpx::id_type const locality_id{hpx::find_here()};
        Element const fill_value{hpx::get_locality_id()};

        partitions[0] = hpx::async(

                [locality_id, offset=offsets[0], partition_shape=partition_shapes[0], fill_value]()
                {
                    return Partition{locality_id, offset, partition_shape, fill_value};
                }

            );

        for(std::size_t idx = 1; idx < nr_partitions; ++idx)
        {
            partitions[idx] = partitions[idx-1].then(

                    [locality_id, offset=offsets[idx], partition_shape=partition_shapes[idx], fill_value](
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
            >::type
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


            template<
                typename Policies>
            hpx::future<std::vector<Partition>> instantiate(
                hpx::id_type const locality_id,
                [[maybe_unused]] Policies const& policies,
                std::vector<Offset> offsets,
                std::vector<Shape> partition_shapes)
            {
                // Spawn a task that will create all partitions for. It must
                // immediately return a collection of partitions which we
                // can assign to the correct place in the array.

                using Action = NumberPartitionsPerLocalityAction</* Policies, */ Partition>;
                Action action{};

                return hpx::async(

                        [locality_id, action=std::move(action),
                            offsets=std::move(offsets), partition_shapes=std::move(partition_shapes)]()
                        {
                            return action(locality_id, offsets, partition_shapes);
                        }

                    );
            }

        private:

    };

}  // Anonymous namespace


namespace lue {

    template<
        typename Element,
        Rank rank>
    class FunctorTraits<
        NumberPartitionsIndividually<Element, rank>>
    {

        public:

            static constexpr bool const is_functor{true};

    };

}  // namespace lue


namespace lue {

    template<
        typename Element,
        Rank rank>
    class FunctorTraits<
        NumberPartitionsPerLocality<Element, rank>>
    {

        public:

            static constexpr bool const is_functor{true};

    };

}  // namespace lue


BOOST_AUTO_TEST_CASE(instantiate_partitions_individually)
{
    // Create a partitioned array in which the elements of the partitions
    // have a unique value per partition. Each partition contains
    // different numbers. The elements within each partition have the
    // same number.

    using Functor = NumberPartitionsIndividually<Element, rank>;

    Array array = lue::create_partitioned_array(array_shape, partition_shape, Functor{});

    // The number of unique values per partition is 1
    // Per partition a single count >= 0
    lue::PartitionedArray<lue::Count, 2> counts = lue::partition_count_unique(array);
    BOOST_CHECK(lue::all(lue::equal_to(counts, lue::Count{1})).get());

    // The number of unique values in the array equals the number of partitions
    lue::PartitionedArray<lue::Count, 1> unique_values = lue::unique(array).get();
    BOOST_CHECK_EQUAL(lue::nr_elements(unique_values), lue::nr_partitions(array));
}


BOOST_AUTO_TEST_CASE(instantiate_partitions_per_locality)
{
    // Create a partitioned array in which the elements of the partitions
    // have a unique value per partition per locality. The partitions
    // instantiated within the same locality contain the same numbers. The
    // elements within each partition have the same number.

    using Functor = NumberPartitionsPerLocality<Element, rank>;

    Array array = lue::create_partitioned_array(array_shape, partition_shape, Functor{});

    // The number of unique values per partition is 1
    lue::PartitionedArray<lue::Count, rank> counts = lue::partition_count_unique(array);
    BOOST_CHECK(lue::all(lue::equal_to(counts, lue::Count{1})).get());

    // The number of unique values in the array equals the number of localities
    lue::PartitionedArray<lue::Count, 1> unique_values = lue::unique(array).get();
    BOOST_CHECK_EQUAL(lue::nr_elements(unique_values), hpx::get_num_localities().get());
}


BOOST_AUTO_TEST_CASE(clamp_mode_merge)
{
    // Create a partitioned array and verify the partitions at the side
    // have merged

    lue::Count const nr_rows = 30;
    lue::Count const nr_cols = 40;
    Shape const array_shape{{nr_rows, nr_cols}};

    // 2 x 2 partitions, with clamping
    lue::Count const nr_rows_partition = 20;
    lue::Count const nr_cols_partition = 30;
    Shape const partition_shape{{nr_rows_partition, nr_cols_partition}};

    using Functor = NumberPartitionsIndividually<Element, rank>;

    Array array = lue::create_partitioned_array(array_shape, partition_shape, Functor{});

    BOOST_CHECK_EQUAL(array.nr_elements(), nr_rows * nr_cols);
    BOOST_CHECK_EQUAL(array.shape(), array_shape);
    BOOST_CHECK_EQUAL(array.nr_partitions(), 1);

    auto const& partitions = array.partitions();
    lue::wait_all(partitions);

    BOOST_CHECK_EQUAL(partitions(0, 0).shape().get(), array_shape);
    BOOST_CHECK_EQUAL((partitions(0, 0).offset().get()), Offset({ 0,  0}));
}


BOOST_AUTO_TEST_CASE(all_no_data)
{
    // TODO Depends on gh297 for easy creation of DefaultValuePolicies
    BOOST_WARN(false);
}


BOOST_AUTO_TEST_CASE(use_case_1)
{
    // Create a partitioned array filled with a value and verify that
    // all cells contain this fill value. Even cells in partitions at the
    // border of the array.

    lue::Count const nr_rows = 300;
    lue::Count const nr_cols = 400;
    Shape const array_shape{{nr_rows, nr_cols}};

    lue::Count const nr_rows_partition = 43;
    lue::Count const nr_cols_partition = 43;
    Shape const partition_shape{{nr_rows_partition, nr_cols_partition}};

    Element const fill_value{33};

    Array array = lue::create_partitioned_array(array_shape, partition_shape, fill_value);

    BOOST_CHECK(lue::all(lue::equal_to(array, fill_value)).get());
}
