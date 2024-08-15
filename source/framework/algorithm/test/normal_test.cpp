#define BOOST_TEST_MODULE lue framework algorithm normal
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/comparison.hpp"
#include "lue/framework/algorithm/default_policies/logical_and.hpp"
#include "lue/framework/algorithm/default_policies/normal.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace detail {

    template<typename Element, std::size_t rank>
    void test_array()
    {
        using namespace lue::default_policies;

        using Array = lue::PartitionedArray<Element, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Array array{lue::create_partitioned_array<Element>(array_shape, partition_shape)};

        Element mean{7};
        Element stddev{9};

        // Fill two arrays with values from a normal distribution and check
        // whether
        // - All cells in these arrays are > mean - 10 * stddev
        // - All cells in these arrays are < mean + 10 * stddev
        // (- All cells in these arrays are different)

        array = normal(array, mean, stddev);

        BOOST_CHECK(all(array > mean - (10 * stddev)).future().get());
        BOOST_CHECK(all(array < mean + (10 * stddev)).future().get());
    }

}  // namespace detail


#define TEST_CASE(rank, Element)                                                                             \
                                                                                                             \
    BOOST_AUTO_TEST_CASE(array_##rank##d_##Element)                                                          \
    {                                                                                                        \
        detail::test_array<Element, rank>();                                                                 \
    }

TEST_CASE(2, double)

#undef TEST_CASE


namespace {

    using Element = float;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;
    // using Offset = lue::OffsetT<Array>;

    // Shape const array_shape{{60, 40}};
    // Shape const partition_shape{{10, 10}};
}  // namespace


BOOST_AUTO_TEST_CASE(use_case_1)
{
    // Create a partitioned array filled with random values and verify that all cells contain
    // correct values

    using namespace lue::default_policies;

    lue::Count const nr_rows = 300;
    lue::Count const nr_cols = 400;
    Shape const array_shape{{nr_rows, nr_cols}};

    lue::Count const nr_rows_partition = 43;
    lue::Count const nr_cols_partition = 43;
    Shape const partition_shape{{nr_rows_partition, nr_cols_partition}};

    Element const mean{33};
    Element const stddev{55};

    Array array = normal(array_shape, partition_shape, mean, stddev);

    BOOST_CHECK(all(array > mean - (10 * stddev)).future().get());
    BOOST_CHECK(all(array < mean + (10 * stddev)).future().get());
}
