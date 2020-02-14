#define BOOST_TEST_MODULE lue framework io write
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/io/write.hpp"
#include "lue/framework/algorithm/uniform.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include <boost/filesystem.hpp>


BOOST_AUTO_TEST_CASE(write_array_int32_2)
{
    using Element = std::int32_t;
    lue::Rank const rank{2};
    using Array = lue::PartitionedArray<Element, rank>;

    auto const shape{lue::Test<Array>::shape()};

    Array array{shape};
    array = lue::uniform(array, 0, 10);

    std::string const pathname{"land_use.tif"};
    lue::write(array, pathname);
    BOOST_CHECK(boost::filesystem::exists(pathname));
}
