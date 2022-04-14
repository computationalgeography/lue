#define BOOST_TEST_MODULE lue framework algorithm aspect
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/aspect.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


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

    auto aspect_we_got = lue::value_policies::aspect(elevation);

    auto aspect_we_want = lue::test::create_partitioned_array<Array>(shape, shape,
        {{
            3.93 , 4.9  , 5.11, nd  , 5.77,
            0.785, 4.96 , 5.06, nd  , nd  ,
            0.326, 6.25 , 5.36, 5.02, 5.38,
            0.086, 0.228, 6.25, 5.58, 5.65,
            6.28 , 0.497, 0.1 , 5.65, 5.83,
        }});

    // TODO Pass in policies to take no-data into account
    // TODO Update algorithm to fill no-data around focal cell
    // lue::test::check_arrays_are_close(aspect_we_got, aspect_we_want, 1e-3);
}
