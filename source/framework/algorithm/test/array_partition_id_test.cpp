#define BOOST_TEST_MODULE lue framework algorithm array_partition_id
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/array_partition_id.hpp"
#include "lue/framework/algorithm/all.hpp"
#include "lue/framework/algorithm/comparison.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace detail {

template<
    typename Element,
    std::size_t rank>
void test_array()
{
    using Array = lue::PartitionedArray<Element, rank>;

    auto const shape{lue::Test<Array>::shape()};

    Array array{shape};

    // Request the IDs of each partition
    auto array_partition_id = lue::array_partition_id(array);

    BOOST_CHECK_EQUAL(array_partition_id.shape(), array.shape());

    auto const& localities{array_partition_id.localities()};
    auto const& partitions{array_partition_id.partitions()};

    for(lue::Index p = 0; p < lue::nr_partitions(array_partition_id); ++p)
    {
        BOOST_CHECK(
            lue::all(
                    localities[p],
                    lue::equal_to(localities[p], partitions[p], static_cast<std::uint64_t>(p))
                ).get());
    }
}

}  // namespace detail


#define TEST_CASE(                               \
    rank,                                        \
    Element)                                     \
                                                 \
BOOST_AUTO_TEST_CASE(array_##rank##d_##Element)  \
{                                                \
    detail::test_array<Element, rank>();         \
}

TEST_CASE(1, int32_t)
TEST_CASE(2, int32_t)

#undef TEST_CASE
