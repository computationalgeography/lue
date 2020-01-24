#define BOOST_TEST_MODULE lue framework core array_partition_data
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/test/stream.hpp"
#include <algorithm>


namespace {

using Value = std::int32_t;
std::size_t const rank = 2;
using Data = lue::ArrayPartitionData<Value, rank>;
using Shape = Data::Shape;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(default_construct)
{
    // Default initialization
    {
        Data data;

        Shape shape{};
        BOOST_CHECK_EQUAL(data.shape(), shape);

        BOOST_CHECK(data.empty());
        BOOST_CHECK_EQUAL(data.nr_elements(), 0);
        BOOST_CHECK(data.begin() == data.end());
    }

    // Value intialization
    {
        Data data{};

        Shape shape{};
        BOOST_CHECK_EQUAL(data.shape(), shape);

        BOOST_CHECK(data.empty());
        BOOST_CHECK_EQUAL(data.nr_elements(), 0);
        BOOST_CHECK(data.begin() == data.end());
    }
}


BOOST_AUTO_TEST_CASE(construct_initialized_definition)
{
    Shape shape{{30, 40}};
    Data data{shape, lue::scattered_target_index()};

    BOOST_CHECK_EQUAL(data.shape(), shape);

    BOOST_CHECK(!data.empty());
    BOOST_CHECK_EQUAL(data.nr_elements(), 30 * 40);

    std::vector<Value> values(30 * 40, 0);
    BOOST_CHECK_EQUAL_COLLECTIONS(
        data.begin(), data.end(), values.begin(), values.end());
}


BOOST_AUTO_TEST_CASE(construct_initialized_definition_and_value)
{
    Shape shape{{30, 40}};
    Data data{shape, 5, lue::scattered_target_index()};

    BOOST_CHECK_EQUAL(data.shape(), shape);

    BOOST_CHECK(!data.empty());
    BOOST_CHECK_EQUAL(data.nr_elements(), 30 * 40);

    std::vector<Value> values(30 * 40, 5);
    BOOST_CHECK_EQUAL_COLLECTIONS(
        data.begin(), data.end(), values.begin(), values.end());
}


BOOST_AUTO_TEST_CASE(construct_initializer_list)
{
    Shape shape{{3, 2}};
    Data data{shape, {
        1, 2,
        3, 4,
        5, 6,
    }, scattered_target_index()};

    BOOST_CHECK_EQUAL(data.shape(), shape);

    BOOST_CHECK(!data.empty());
    BOOST_CHECK_EQUAL(data.nr_elements(), 6);

    BOOST_CHECK_EQUAL(data(0, 0), 1);
    BOOST_CHECK_EQUAL(data(0, 1), 2);
    BOOST_CHECK_EQUAL(data(1, 0), 3);
    BOOST_CHECK_EQUAL(data(1, 1), 4);
    BOOST_CHECK_EQUAL(data(2, 0), 5);
    BOOST_CHECK_EQUAL(data(2, 1), 6);
}


BOOST_AUTO_TEST_CASE(assign)
{
    Data data{};

    Shape shape{{30, 40}};

    data = Data{shape, 5, lue::scattered_target_index()};

    BOOST_CHECK_EQUAL(data.shape(), shape);

    BOOST_CHECK(!data.empty());
    BOOST_CHECK_EQUAL(data.nr_elements(), 30 * 40);

    std::vector<Value> values(30 * 40, 5);
    BOOST_CHECK_EQUAL_COLLECTIONS(
        data.begin(), data.end(), values.begin(), values.end());
}


BOOST_AUTO_TEST_CASE(scalar_array)
{
    std::size_t const rank = 0;
    using Data = lue::ArrayPartitionData<Value, rank>;
    using Shape = Data::Shape;

    Shape shape{};

    {
        Data data;
        BOOST_CHECK_EQUAL(data.shape(), shape);
        BOOST_CHECK(!data.empty());
        BOOST_CHECK_EQUAL(data.nr_elements(), 1);
    }

    {
        Data data{5, lue::scattered_target_index()};
        BOOST_CHECK_EQUAL(data.shape(), shape);
        BOOST_CHECK(!data.empty());
        BOOST_CHECK_EQUAL(data.nr_elements(), 1);
        BOOST_CHECK_EQUAL(data[0], 5);
    }
}


BOOST_AUTO_TEST_CASE(slice_1d)
{
    using Value = std::int32_t;
    std::size_t const rank = 1;
    using Data = lue::ArrayPartitionData<Value, rank>;
    using Shape = Data::Shape;
    std::size_t const nr_elements = 10;
    using Slice = Data::Slice;
    using Slices = Data::Slices;

    Shape shape{{nr_elements}};

    Data data{shape, lue::scattered_target_index()};
    std::iota(data.begin(), data.end(), 0);

    {
        Slices slices{Slice{0, 10}};
        auto data_slice = data.slice(slices);

        BOOST_REQUIRE_EQUAL(data_slice.shape(), shape);
        BOOST_CHECK_EQUAL(data_slice(0), data(0));
        BOOST_CHECK_EQUAL(data_slice(9), data(9));
    }

    {
        Slices slices{Slice{5, 5}};
        auto data_slice = data.slice(slices);

        BOOST_CHECK_EQUAL(data_slice.shape(), Shape{{0}});
        BOOST_CHECK_EQUAL(data_slice.nr_elements(), 0);
    }

    {
        Slices slices{Slice{5, 6}};
        auto data_slice = data.slice(slices);

        BOOST_REQUIRE_EQUAL(data_slice.shape(), Shape{{1}});
        BOOST_CHECK_EQUAL(data_slice(0), data(5));
    }
}


BOOST_AUTO_TEST_CASE(slice_2d)
{
    using Value = std::int32_t;
    std::size_t const rank = 2;
    using Data = lue::ArrayPartitionData<Value, rank>;
    using Shape = Data::Shape;
    std::size_t const nr_rows = 30;
    std::size_t const nr_cols = 20;
    using Slice = Data::Slice;
    using Slices = Data::Slices;

    Shape shape{{nr_rows, nr_cols}};

    Data data{shape, lue::scattered_target_index()};
    std::iota(data.begin(), data.end(), 0);

    {
        Slices slices{Slice{0, nr_rows}, Slice{0, nr_cols}};
        auto data_slice = data.slice(slices);

        BOOST_REQUIRE_EQUAL(data_slice.shape(), shape);
        BOOST_CHECK_EQUAL(data_slice(0, 0), data(0, 0));
        BOOST_CHECK_EQUAL(data_slice(29, 19), data(29, 19));
    }

    {
        Slices slices{Slice{5, 5}, Slice{0, nr_cols}};
        auto data_slice = data.slice(slices);

        BOOST_CHECK_EQUAL(data_slice.shape(), (Shape{{0, nr_cols}}));
        BOOST_CHECK_EQUAL(data_slice.nr_elements(), 0);
    }

    {
        Slices slices{Slice{0, nr_rows}, Slice{6, 6}};
        auto data_slice = data.slice(slices);

        BOOST_CHECK_EQUAL(data_slice.shape(), (Shape{{nr_rows, 0}}));
        BOOST_CHECK_EQUAL(data_slice.nr_elements(), 0);
    }

    {
        Slices slices{Slice{5, 6}, Slice{6, 7}};
        auto data_slice = data.slice(slices);

        BOOST_REQUIRE_EQUAL(data_slice.shape(), (Shape{{1, 1}}));
        BOOST_CHECK_EQUAL(data_slice(0, 0), data(5, 6));
    }
}
