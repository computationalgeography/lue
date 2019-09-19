#define BOOST_TEST_MODULE lue framework algorithm convolve
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/convolve.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(window_total_1d)
{
    using Element = std::int32_t;
    std::size_t const rank = 1;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const shape{{5}};

    Array array{shape};

    auto const kernel = lue::square_kernel<bool, rank>(1, true);

    // lue::convolve(array, kernel);

    // detail::test_array<Element, rank>();
}
