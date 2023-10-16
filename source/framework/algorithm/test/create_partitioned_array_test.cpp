#define BOOST_TEST_MODULE lue framework algorithm create_partitioned_array
#include "boost/smart_ptr/make_shared.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/equal_to.hpp"
#include "lue/framework/algorithm/default_policies/logical_not.hpp"
#include "lue/framework/algorithm/default_policies/unique.hpp"
#include "lue/framework/algorithm/partition_count_unique.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/sum.hpp"
#include "lue/framework/algorithm/value_policies/logical_not.hpp"
#include "lue/framework/algorithm/value_policies/valid.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

    using Element = std::int64_t;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;
    using Offset = lue::OffsetT<Array>;

    Shape const array_shape{{60, 40}};
    Shape const partition_shape{{10, 10}};


    template<typename Element, lue::Rank rank>
    class NumberPartitionsIndividually

    {

        public:

            using OutputElement = Element;
            using Partition = lue::ArrayPartition<OutputElement, rank>;
            using Offset = lue::OffsetT<Partition>;
            using Shape = lue::ShapeT<Partition>;

            static constexpr bool instantiate_per_locality{false};


            template<typename Policies>
            Partition instantiate(
                hpx::id_type const locality_id,
                [[maybe_unused]] Policies const& policies,
                [[maybe_unused]] Shape const& array_shape,
                lue::Index const partition_idx,
                Offset const& offset,
                Shape const& partition_shape)
            {
                Element const fill_value{partition_idx};

                return hpx::async(

                    [locality_id, offset, partition_shape, fill_value]() {
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

            [locality_id, offset = offsets[0], partition_shape = partition_shapes[0], fill_value]() {
                return Partition{locality_id, offset, partition_shape, fill_value};
            }

        );

        for (std::size_t idx = 1; idx < nr_partitions; ++idx)
        {
            partitions[idx] = partitions[idx - 1].then(

                [locality_id, offset = offsets[idx], partition_shape = partition_shapes[idx], fill_value](
                    auto const& /* previous_partition */) {
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
            NumberPartitionsPerLocalityAction</* Policies, */ Partition>>::type
    {
    };


    template<typename Element, lue::Rank rank>
    class NumberPartitionsPerLocality
    {

        public:

            using OutputElement = Element;
            using Partition = lue::ArrayPartition<OutputElement, rank>;
            using Offset = lue::OffsetT<Partition>;
            using Shape = lue::ShapeT<Partition>;

            static constexpr bool instantiate_per_locality{true};


            template<typename Policies>
            hpx::future<std::vector<Partition>> instantiate(
                hpx::id_type const locality_id,
                [[maybe_unused]] Policies const& policies,
                [[maybe_unused]] Shape const& array_shape,
                std::vector<Offset> offsets,
                std::vector<Shape> partition_shapes)
            {
                // Spawn a task that will create all partitions for. It must
                // immediately return a collection of partitions which we
                // can assign to the correct place in the array.

                using Action = NumberPartitionsPerLocalityAction</* Policies, */ Partition>;
                Action action{};

                return hpx::async(

                    [locality_id,
                     action = std::move(action),
                     offsets = std::move(offsets),
                     partition_shapes = std::move(partition_shapes)]()
                    { return action(locality_id, offsets, partition_shapes); }

                );
            }

        private:
    };

}  // Anonymous namespace


namespace lue {

    template<typename Element, Rank rank>
    class FunctorTraits<NumberPartitionsIndividually<Element, rank>>
    {

        public:

            static constexpr bool const is_functor{true};
    };

}  // namespace lue


namespace lue {

    template<typename Element, Rank rank>
    class FunctorTraits<NumberPartitionsPerLocality<Element, rank>>
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
    using namespace lue::default_policies;

    using Functor = NumberPartitionsIndividually<Element, rank>;

    Array array = lue::create_partitioned_array(array_shape, partition_shape, Functor{});

    // The number of unique values per partition is 1
    // Per partition a single count >= 0
    lue::PartitionedArray<std::int64_t, 2> counts = lue::partition_count_unique(array);
    BOOST_CHECK(all(equal_to(counts, std::int64_t{1})).get());

    // The number of unique values in the array equals the number of partitions
    std::set<Element> const unique_values = unique(array).get();
    BOOST_CHECK_EQUAL(unique_values.size(), lue::nr_partitions(array));
}


BOOST_AUTO_TEST_CASE(instantiate_partitions_per_locality)
{
    // Create a partitioned array in which the elements of the partitions
    // have a unique value per partition per locality. The partitions
    // instantiated within the same locality contain the same numbers. The
    // elements within each partition have the same number.

    using namespace lue::default_policies;

    using Functor = NumberPartitionsPerLocality<Element, rank>;

    Array array = lue::create_partitioned_array(array_shape, partition_shape, Functor{});

    // The number of unique values per partition is 1
    lue::PartitionedArray<std::int64_t, rank> counts = lue::partition_count_unique(array);
    BOOST_CHECK(all(equal_to(counts, std::int64_t{1})).get());

    // The number of unique values in the array equals the number of localities
    std::set<Element> const unique_values = unique(array).get();
    BOOST_CHECK_EQUAL(unique_values.size(), hpx::get_num_localities().get());
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
    BOOST_CHECK_EQUAL((partitions(0, 0).offset().get()), Offset({0, 0}));
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

    using namespace lue::default_policies;

    lue::Count const nr_rows = 300;
    lue::Count const nr_cols = 400;
    Shape const array_shape{{nr_rows, nr_cols}};

    lue::Count const nr_rows_partition = 43;
    lue::Count const nr_cols_partition = 43;
    Shape const partition_shape{{nr_rows_partition, nr_cols_partition}};

    Element const fill_value{33};

    Array array = lue::create_partitioned_array(array_shape, partition_shape, fill_value);

    BOOST_CHECK(all(equal_to(array, fill_value)).get());
}


namespace lue::detail {

    template<typename T>
    class ArrayTraits<std::shared_ptr<std::vector<T>>>
    {

        public:

            using Element = T;
    };


    template<typename T>
    class ArrayTraits<boost::shared_ptr<T[]>>
    {

        public:

            using Element = T;
    };

}  // namespace lue::detail


BOOST_AUTO_TEST_CASE(use_case_2)
{
    // Create a partitioned array given an existing buffer containing values to copy into
    // the array

    // Create an object, pointed to by a reference counted object. Complicate things a bit so
    // we can mimic an actual use-case with some foreign data structure.
    std::size_t const nr_rows{60};
    std::size_t const nr_cols{40};
    std::size_t const nr_values{nr_rows * nr_cols};

    using Buffer = std::vector<Element>;
    using BufferHandle = std::shared_ptr<Buffer>;

    BufferHandle buffer_handle = std::make_shared<Buffer>(nr_values);
    BOOST_CHECK_EQUAL(buffer_handle.use_count(), 1);
    std::iota(buffer_handle->begin(), buffer_handle->end(), 0);

    // Create a partitioned array, passing in the buffer
    Shape const array_shape{{nr_rows, nr_cols}};
    Shape const partition_shape{{10, 10}};

    using Functor = lue::InstantiateFromBuffer<BufferHandle, rank>;

    Functor functor{
        buffer_handle,  // Copy: increments reference count
        [](BufferHandle const& handle) -> Element* { return handle->data(); }

    };

    Array array = lue::create_partitioned_array(array_shape, partition_shape, functor);
    BOOST_CHECK_GE(buffer_handle.use_count(), 2);
    BOOST_CHECK_LE(buffer_handle.use_count(), 2 + 6 * 4);

    BOOST_CHECK_EQUAL(array.nr_elements(), nr_rows * nr_cols);
    BOOST_CHECK_EQUAL(array.shape(), array_shape);
    BOOST_REQUIRE_EQUAL(array.nr_partitions(), 6 * 4);

    auto const& partitions = array.partitions();
    lue::wait_all(partitions);

    // buffer_handle and the functor contain a copy of buffer_handle
    // At least once this test failed (use_count was 3). Could be this specific test is wrong.
    BOOST_CHECK_EQUAL(buffer_handle.use_count(), 2);

    auto const [nr_partitions0, nr_partitions1] = array.partitions().shape();

    for (lue::Index partition0 = 0; partition0 < nr_partitions0; ++partition0)
    {
        for (lue::Index partition1 = 0; partition1 < nr_partitions1; ++partition1)
        {
            BOOST_TEST_CONTEXT(fmt::format("Partition {}, {}", partition0, partition1))
            {
                auto const& partition{partitions(partition0, partition1)};
                auto const data{partition.data().get()};
                auto const [nr_cells0, nr_cells1] = data.shape();

                for (lue::Index cell0 = 0; cell0 < nr_cells0; ++cell0)
                {
                    for (lue::Index cell1 = 0; cell1 < nr_cells1; ++cell1)
                    {
                        BOOST_TEST_CONTEXT(fmt::format("Cell {}, {}", cell0, cell1))
                        {
                            BOOST_CHECK_EQUAL(
                                data(cell0, cell1),
                                // previous rows:
                                ((partition0 * partition_shape[0]) + cell0) * array_shape[1] +
                                    // previous cols:
                                    (partition1 * partition_shape[1]) + cell1);
                        }
                    }
                }
            }
        }
    }
}


BOOST_AUTO_TEST_CASE(use_case_3)
{
    // Create a partitioned array given an existing buffer containing values to copy into
    // the array. Mark specific values from the buffer as no-data in the array.

    using namespace lue::value_policies;

    // Create an object, pointed to by a reference counted object
    std::size_t const nr_rows{60};
    std::size_t const nr_cols{40};
    std::size_t const nr_values{nr_rows * nr_cols};

    using Buffer = Element[];
    using BufferHandle = boost::shared_ptr<Buffer>;

    BufferHandle buffer_handle = boost::make_shared<Buffer>(nr_values);
    BOOST_CHECK_EQUAL(buffer_handle.use_count(), 1);
    std::fill_n(buffer_handle.get(), nr_values, 5);

    // Put a special no-data value in the first cell of each partition
    for (lue::Index i0 = 0; i0 < 6; ++i0)
    {
        for (lue::Index i1 = 0; i1 < 4; ++i1)
        {
            buffer_handle.get()[i0 * 10 * nr_cols + i1 * 10] = 999;
        }
    }

    // Create a partitioned array, passing in the buffer
    Shape const array_shape{{nr_rows, nr_cols}};
    Shape const partition_shape{{10, 10}};

    using Functor = lue::InstantiateFromBuffer<BufferHandle, rank>;

    Functor functor{
        buffer_handle,  // Copy: increments reference count
        [](BufferHandle const& handle) -> Element* { return handle.get(); },
        999};
    BOOST_CHECK_EQUAL(buffer_handle.use_count(), 2);

    using CreatePartitionedArrayPolicies =
        lue::policy::create_partitioned_array::DefaultValuePolicies<Element>;
    Array array = lue::create_partitioned_array(
        CreatePartitionedArrayPolicies{}, array_shape, partition_shape, functor);

    // In each partition we put a no-data value, so the number of no-data values must be equal
    // to the number of partitions.
    BOOST_CHECK_EQUAL(lue::sum(!valid<std::uint8_t>(array)).get(), array.nr_partitions());

    BOOST_CHECK_EQUAL(buffer_handle.use_count(), 2);

    // Now test these explicitly. All other values must then be valid, by definition.
    auto const [nr_partitions0, nr_partitions1] = array.partitions().shape();

    lue::policy::DefaultOutputNoDataPolicy<Element> ondp{};

    for (lue::Index partition0 = 0; partition0 < nr_partitions0; ++partition0)
    {
        for (lue::Index partition1 = 0; partition1 < nr_partitions1; ++partition1)
        {
            BOOST_TEST_CONTEXT(fmt::format("Partition {}, {}", partition0, partition1))
            {
                auto const& partition{array.partitions()(partition0, partition1)};
                auto const data{partition.data().get()};

                BOOST_CHECK(ondp.is_no_data(data, 0, 0));
                BOOST_CHECK(!ondp.is_no_data(data, 0, 1));
            }
        }
    }
}
