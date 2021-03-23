#define BOOST_TEST_MODULE lue framework algorithm slope
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/focal.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/define.hpp"  // LUE_UNUSED


BOOST_AUTO_TEST_CASE(use_case_1)
{
    // Create input raster (from the PCRaster manual):
    // +-----+-----+-----+-----+-----+
    // |  70 |  70 |  80 |  X  | 120 |
    // +-----+-----+-----+-----+-----+
    // |  70 |  70 |  90 |  X  |  X  |
    // +-----+-----+-----+-----+-----+
    // |  70 |  70 | 100 | 140 | 280 |
    // +-----+-----+-----+-----+-----+
    // | 180 | 160 | 110 | 160 | 320 |
    // +-----+-----+-----+-----+-----+
    // | 510 | 440 | 300 | 400 | 480 |
    // +-----+-----+-----+-----+-----+
    lue::Count const nr_rows{5};
    lue::Count const nr_cols{5};

    using Element = double;
    lue::Rank const rank{2};

    using Array = lue::PartitionedArray<Element, rank>;
    using Partition = lue::PartitionT<Array>;
    using PartitionData = lue::DataT<Partition>;
    using Shape = lue::ShapeT<Array>;

    Shape shape{nr_rows, nr_cols};

    Element const nd{std::numeric_limits<Element>::quiet_NaN()};

    auto elevation = lue::test::create_partitioned_array<Array>(shape, shape,
        {{
             70,  70,  80,  nd, 120,
             70,  70,  90,  nd,  nd,
             70,  70, 100, 140, 280,
            180, 160, 110, 160, 320,
            510, 440, 300, 400, 480,
        }});

    Element cell_size = 50.0;

    auto slope_we_got = lue::slope(elevation, cell_size);
    LUE_UNUSED(slope_we_got);

    auto slope_we_want = lue::test::create_partitioned_array<Array>(shape, shape,
        {{
            0.0118, 0.114, 0.394, nd  , 0.673,
            0.13  , 0.206, 0.604, nd  , nd,
            1.3   , 0.775, 0.643, 1.73, 1.87,
            3.73  , 3.54 , 2.58 , 3.02, 2.36,
            2.76  , 3.07 , 2.59 , 2.66, 1.65,
        }});

    // Array slope_we_want{lue::create_partitioned_array<Element>(shape, shape)};
    // slope_we_want.partitions()(0, 0).wait();
    // slope_we_want.partitions()(0, 0).set_data(std::move(data));

    BOOST_WARN_MESSAGE(false, "Update slope to use policies");

    // FIXME Update focal_operation to use policies that are needed by slope
    // lue::test::check_arrays_are_close(slope_we_got, slope_we_want);
}
