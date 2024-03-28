#define BOOST_TEST_MODULE lue framework algorithm clump
#include "lue/framework/algorithm/value_policies/clump.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(clump)
{
    using namespace lue::value_policies;

    lue::Shape<lue::Count, 2> const array_shape{3, 3};
    lue::Shape<lue::Count, 2> const partition_shape{3, 3};

    using ZoneElement = std::int32_t;

    auto const zone_nd{lue::no_data<ZoneElement>};

    auto const zone = lue::test::create_partitioned_array<lue::PartitionedArray<ZoneElement, 2>>(
        array_shape,
        partition_shape,
        {{
            0, zone_nd, 1,
            1, 0, 1,
            1, zone_nd, 0,
        }});

    // TODO
    // {
    //     auto const result_we_want =
    //         lue::test::create_partitioned_array<lue::PartitionedArray<IndexElement, 2>>(
    //             array_shape,
    //             partition_shape,
    //             {{
    //                 index_nd,
    //                 index_nd,
    //                 0,
    //                 1,
    //                 index_nd,
    //                 1,
    //                 2,
    //                 index_nd,
    //                 index_nd,
    //             }});

    //     auto const result_we_got = lue::value_policies::cell_index<IndexElement>(condition, 0);

    //     lue::test::check_arrays_are_equal(result_we_got, result_we_want);
    // }

    // {
    //     auto const result_we_want =
    //         lue::test::create_partitioned_array<lue::PartitionedArray<IndexElement, 2>>(
    //             array_shape,
    //             partition_shape,
    //             {{
    //                 index_nd,
    //                 index_nd,
    //                 2,
    //                 0,
    //                 index_nd,
    //                 2,
    //                 0,
    //                 index_nd,
    //                 index_nd,
    //             }});

    //     auto const result_we_got = lue::value_policies::cell_index<IndexElement>(condition, 1);

    //     lue::test::check_arrays_are_equal(result_we_got, result_we_want);
    // }
}
