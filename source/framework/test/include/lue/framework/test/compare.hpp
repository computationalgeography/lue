#pragma once
#include "lue/framework/test/stream.hpp"
#include "lue/framework/core/span.hpp"
#include <boost/range/irange.hpp>
#include <boost/test/test_tools.hpp>


namespace std {
namespace experimental {

template<
    typename Element,
    lue::Rank rank>
bool compare(
    lue::DynamicSpan<Element, rank> const& lhs,
    lue::DynamicSpan<Element, rank> const& rhs)
{
    // Both spans are equal if the extents in all dimensions are
    // equal. This means that finding a dimension for which this is not
    // the case must not result in a match.

    auto const idxs = boost::irange<lue::Index>(0, rank);

    return std::find_if(idxs.begin(), idxs.end(),

            [&lhs, &rhs](lue::Index const idx)
            {
                return lhs.extent(idx) != rhs.extent(idx);
            }

        ) == idxs.end();
}


template<
    typename Element>
bool operator==(
    lue::DynamicSpan<Element, 2> const& lhs,
    lue::DynamicSpan<Element, 2> const& rhs)
{
    return compare<Element, 2>(lhs, rhs);
}

}  // namespace experimental
}  // namespace std


namespace lue {
namespace test {

template<
    typename PartitionData>
void check_equal_array_partition_data(
    PartitionData const& partition_data1,
    PartitionData const& partition_data2)
{
    BOOST_REQUIRE_EQUAL(partition_data1.shape(), partition_data2.shape());
    BOOST_REQUIRE_EQUAL(partition_data1.span(), partition_data2.span());

    BOOST_CHECK_EQUAL_COLLECTIONS(
        partition_data1.begin(), partition_data1.end(),
        partition_data2.begin(), partition_data2.end());
}


template<
    typename Partition>
void check_equal_partition(
    Partition const& partition1,
    Partition const& partition2)
{
    bool both_partitions_valid = partition1.valid() && partition2.valid();
    bool both_partitions_invalid =
        (!partition1.valid()) && (!partition2.valid());

    // XOR with bools
    BOOST_REQUIRE(both_partitions_valid != both_partitions_invalid);

    if(both_partitions_valid) {
        BOOST_REQUIRE(partition1.get_id());
        BOOST_REQUIRE(partition2.get_id());

        BOOST_REQUIRE_EQUAL(
            partition1.shape().get(), partition2.shape().get());

        check_equal_array_partition_data(
            partition1.data(CopyMode::copy).get(),
            partition2.data(CopyMode::copy).get());
    }
}


template<
    typename Partitions>
void check_equal_partitions(
    Partitions const& partitions1,
    Partitions const& partitions2)
{
    BOOST_REQUIRE_EQUAL(partitions1.shape(), partitions2.shape());

    for(Index p = 0; p < partitions1.nr_elements(); ++p) {
        BOOST_TEST_CONTEXT(fmt::format("Partition index: {}", p))
        check_equal_partition(partitions1[p], partitions2[p]);
    }
}


template<
    typename Element,
    Rank rank>
void check_equal_array(
    PartitionedArray<Element, rank> const& array1,
    PartitionedArray<Element, rank> const& array2)
{
    BOOST_REQUIRE_EQUAL(
        shape_in_partitions(array1),
        shape_in_partitions(array2));
    BOOST_REQUIRE_EQUAL(array1.shape(), array2.shape());

    check_equal_partitions(array1.partitions(), array2.partitions());
}

}  // namespace test
}  // namespace lue
