#define BOOST_TEST_MODULE lue framework core array_partition_data
#include <hpx/config.hpp>
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/test/stream.hpp"
#include <algorithm>


namespace {

using Value = std::int32_t;

template<
    lue::Rank rank>
using Data = lue::ArrayPartitionData<Value, rank>;

template<
    lue::Rank rank>
using Shape = lue::ShapeT<Data<rank>>;

template<
    lue::Rank rank>
using Slice = lue::SliceT<Data<rank>>;

template<
    lue::Rank rank>
using Slices = lue::SlicesT<Data<rank>>;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(default_construct)
{
    // Default initialization
    {
        Data<2> data;

        Shape<2> shape{};
        BOOST_CHECK_EQUAL(data.shape(), shape);

        BOOST_CHECK(data.empty());
        BOOST_CHECK_EQUAL(data.nr_elements(), 0);
        BOOST_CHECK(data.begin() == data.end());
    }

    // Value intialization
    {
        Data<2> data{};

        Shape<2> shape{};
        BOOST_CHECK_EQUAL(data.shape(), shape);

        BOOST_CHECK(data.empty());
        BOOST_CHECK_EQUAL(data.nr_elements(), 0);
        BOOST_CHECK(data.begin() == data.end());
    }
}


BOOST_AUTO_TEST_CASE(construct_initialized_definition)
{
    Shape<2> shape{{30, 40}};
    Data<2> data{shape};

    BOOST_CHECK_EQUAL(data.shape(), shape);

    BOOST_CHECK(!data.empty());
    BOOST_CHECK_EQUAL(data.nr_elements(), 30 * 40);

    std::vector<Value> values(30 * 40, 0);
    BOOST_CHECK_EQUAL_COLLECTIONS(
        data.begin(), data.end(), values.begin(), values.end());
}


BOOST_AUTO_TEST_CASE(construct_initialized_definition_and_value)
{
    Shape<2> shape{{30, 40}};
    Data<2> data{shape, 5};

    BOOST_CHECK_EQUAL(data.shape(), shape);

    BOOST_CHECK(!data.empty());
    BOOST_CHECK_EQUAL(data.nr_elements(), 30 * 40);

    std::vector<Value> values(30 * 40, 5);
    BOOST_CHECK_EQUAL_COLLECTIONS(
        data.begin(), data.end(), values.begin(), values.end());
}


BOOST_AUTO_TEST_CASE(construct_initializer_list)
{
    Shape<2> shape{{3, 2}};
    Data<2> data{shape, {
        1, 2,
        3, 4,
        5, 6,
    }};

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
    Data<2> data{};

    Shape<2> shape{{30, 40}};

    data = Data<2>{shape, 5};

    BOOST_CHECK_EQUAL(data.shape(), shape);

    BOOST_CHECK(!data.empty());
    BOOST_CHECK_EQUAL(data.nr_elements(), 30 * 40);

    std::vector<Value> values(30 * 40, 5);
    BOOST_CHECK_EQUAL_COLLECTIONS(
        data.begin(), data.end(), values.begin(), values.end());
}


BOOST_AUTO_TEST_CASE(scalar_array)
{
    Shape<0> shape{};

    {
        Data<0> data;
        BOOST_CHECK_EQUAL(data.shape(), shape);
        BOOST_CHECK(!data.empty());
        BOOST_CHECK_EQUAL(data.nr_elements(), 1);
    }

    {
        Data<0> data{5};
        BOOST_CHECK_EQUAL(data.shape(), shape);
        BOOST_CHECK(!data.empty());
        BOOST_CHECK_EQUAL(data.nr_elements(), 1);
        BOOST_CHECK_EQUAL(data[0], 5);
    }
}


BOOST_AUTO_TEST_CASE(slice_1d)
{
    std::size_t const nr_elements = 10;

    Shape<1> shape{{nr_elements}};

    Data<1> data{shape};
    std::iota(data.begin(), data.end(), 0);

    {
        Slices<1> slices{Slice<1>{0, 10}};
        auto data_slice = data.slice(slices);

        BOOST_REQUIRE_EQUAL(data_slice.shape(), shape);
        BOOST_CHECK_EQUAL(data_slice(0), data(0));
        BOOST_CHECK_EQUAL(data_slice(9), data(9));
    }

    {
        Slices<1> slices{Slice<1>{5, 5}};
        auto data_slice = data.slice(slices);

        BOOST_CHECK_EQUAL(data_slice.shape(), Shape<1>{{0}});
        BOOST_CHECK_EQUAL(data_slice.nr_elements(), 0);
    }

    {
        Slices<1> slices{Slice<1>{5, 6}};
        auto data_slice = data.slice(slices);

        BOOST_REQUIRE_EQUAL(data_slice.shape(), Shape<1>{{1}});
        BOOST_CHECK_EQUAL(data_slice(0), data(5));
    }
}


BOOST_AUTO_TEST_CASE(slice_2d)
{
    std::size_t const nr_rows = 30;
    std::size_t const nr_cols = 20;

    Shape<2> shape{{nr_rows, nr_cols}};

    Data<2> data{shape};
    std::iota(data.begin(), data.end(), 0);

    {
        Slices<2> slices{Slice<2>{0, nr_rows}, Slice<2>{0, nr_cols}};
        auto data_slice = data.slice(slices);

        BOOST_REQUIRE_EQUAL(data_slice.shape(), shape);
        BOOST_CHECK_EQUAL(data_slice(0, 0), data(0, 0));
        BOOST_CHECK_EQUAL(data_slice(29, 19), data(29, 19));
    }

    {
        Slices<2> slices{Slice<2>{5, 5}, Slice<2>{0, nr_cols}};
        auto data_slice = data.slice(slices);

        BOOST_CHECK_EQUAL(data_slice.shape(), (Shape<2>{{0, nr_cols}}));
        BOOST_CHECK_EQUAL(data_slice.nr_elements(), 0);
    }

    {
        Slices<2> slices{Slice<2>{0, nr_rows}, Slice<2>{6, 6}};
        auto data_slice = data.slice(slices);

        BOOST_CHECK_EQUAL(data_slice.shape(), (Shape<2>{{nr_rows, 0}}));
        BOOST_CHECK_EQUAL(data_slice.nr_elements(), 0);
    }

    {
        Slices<2> slices{Slice<2>{5, 6}, Slice<2>{6, 7}};
        auto data_slice = data.slice(slices);

        BOOST_REQUIRE_EQUAL(data_slice.shape(), (Shape<2>{{1, 1}}));
        BOOST_CHECK_EQUAL(data_slice(0, 0), data(5, 6));
    }
}


BOOST_AUTO_TEST_CASE(deep_copy_0d)
{
    Shape<0> const shape{};

    Data<0> source_data{shape};
    std::iota(source_data.begin(), source_data.end(), 9);

    Data<0> copied_data{deep_copy(source_data)};

    BOOST_CHECK(source_data.begin() != copied_data.begin());
    BOOST_CHECK_EQUAL(source_data, copied_data);
}


BOOST_AUTO_TEST_CASE(deep_copy_1d)
{
    lue::Count const nr_elements{5};
    Shape<1> const shape{nr_elements};

    Data<1> source_data{shape};
    std::iota(source_data.begin(), source_data.end(), 9);

    Data<1> copied_data{deep_copy(source_data)};

    BOOST_CHECK(source_data.begin() != copied_data.begin());
    BOOST_CHECK_EQUAL(source_data, copied_data);
}


BOOST_AUTO_TEST_CASE(deep_copy_2d)
{
    lue::Count const nr_elements0{5};
    lue::Count const nr_elements1{6};
    Shape<2> const shape{nr_elements0, nr_elements1};

    Data<2> source_data{shape};
    std::iota(source_data.begin(), source_data.end(), 9);

    Data<2> copied_data{deep_copy(source_data)};

    BOOST_CHECK(source_data.begin() != copied_data.begin());
    BOOST_CHECK_EQUAL(source_data, copied_data);
}


BOOST_AUTO_TEST_CASE(erase_1d)
{
    lue::Count const nr_elements{5};
    Shape<1> const shape{nr_elements};

    // TODO Also check values

    // +---+---+---+---+---+
    // | x | x | x | x | x |
    // +---+---+---+---+---+
    Data<1> data{shape};
    std::iota(data.begin(), data.end(), 10);
    BOOST_CHECK_EQUAL(data.nr_elements(), 5);

    // Empty hyperslab
    data.erase(0, 0, 0);
    BOOST_CHECK_EQUAL(data.nr_elements(), 5);

    // Last element
    // +---+---+---+---+  ---+
    // | x | x | x | x |   x |
    // +---+---+---+---+  ---+
    data.erase(0, 4, 5);
    BOOST_CHECK_EQUAL(data.nr_elements(), 4);
    BOOST_CHECK_EQUAL(data[0], 10);
    BOOST_CHECK_EQUAL(data[3], 13);

    // Third element
    // +---+---+  ---  +---+
    // | x | x |   x   | x |
    // +---+---+  ---  +---+
    data.erase(0, 2, 3);
    BOOST_CHECK_EQUAL(data.nr_elements(), 3);
    BOOST_CHECK_EQUAL(data[0], 10);
    BOOST_CHECK_EQUAL(data[1], 11);
    BOOST_CHECK_EQUAL(data[2], 13);

    // All the rest
    data.erase(0, 0, data.nr_elements());
    BOOST_CHECK_EQUAL(data.nr_elements(), 0);
}


BOOST_AUTO_TEST_CASE(erase_2d)
{
    lue::Count const nr_elements0{5};
    lue::Count const nr_elements1{6};
    Shape<2> const shape{nr_elements0, nr_elements1};

    // +----+----+----+----+----+----+
    // | 10 | 11 | 12 | 13 | 14 | 15 |
    // +----+----+----+----+----+----+
    // | 16 | 17 | 18 | 19 | 20 | 21 |
    // +----+----+----+----+----+----+
    // | 22 | 23 | 24 | 25 | 26 | 27 |
    // +----+----+----+----+----+----+
    // | 28 | 29 | 30 | 31 | 32 | 33 |
    // +----+----+----+----+----+----+
    // | 34 | 35 | 36 | 37 | 38 | 39 |
    // +----+----+----+----+----+----+
    Data<2> data{shape};
    std::iota(data.begin(), data.end(), 10);
    BOOST_CHECK_EQUAL(data.nr_elements(), 30);

    // Empty hyperslab
    data.erase(0, 2, 2);
    BOOST_CHECK_EQUAL(data.nr_elements(), 30);

    data.erase(1, 2, 2);
    BOOST_CHECK_EQUAL(data.nr_elements(), 30);

    // Last element, along first dimension
    // +----+----+----+----+----+----+
    // | 10 | 11 | 12 | 13 | 14 | 15 |
    // +----+----+----+----+----+----+
    // | 16 | 17 | 18 | 19 | 20 | 21 |
    // +----+----+----+----+----+----+
    // | 22 | 23 | 24 | 25 | 26 | 27 |
    // +----+----+----+----+----+----+
    // | 28 | 29 | 30 | 31 | 32 | 33 |
    // +----+----+----+----+----+----+
    //
    // | 34 | 35 | 36 | 37 | 38 | 39 |
    // +----+----+----+----+----+----+
    data.erase(0, 4, 5);
    BOOST_CHECK_EQUAL(data.shape()[0], 4);
    BOOST_CHECK_EQUAL(data.shape()[1], 6);
    BOOST_CHECK_EQUAL(data[ 0], 10);
    BOOST_CHECK_EQUAL(data[ 1], 11);
    BOOST_CHECK_EQUAL(data[ 2], 12);
    BOOST_CHECK_EQUAL(data[ 3], 13);
    BOOST_CHECK_EQUAL(data[ 4], 14);
    BOOST_CHECK_EQUAL(data[ 5], 15);
    BOOST_CHECK_EQUAL(data[ 6], 16);
    BOOST_CHECK_EQUAL(data[ 7], 17);
    BOOST_CHECK_EQUAL(data[ 8], 18);
    BOOST_CHECK_EQUAL(data[ 9], 19);
    BOOST_CHECK_EQUAL(data[10], 20);
    BOOST_CHECK_EQUAL(data[11], 21);
    BOOST_CHECK_EQUAL(data[12], 22);
    BOOST_CHECK_EQUAL(data[13], 23);
    BOOST_CHECK_EQUAL(data[14], 24);
    BOOST_CHECK_EQUAL(data[15], 25);
    BOOST_CHECK_EQUAL(data[16], 26);
    BOOST_CHECK_EQUAL(data[17], 27);
    BOOST_CHECK_EQUAL(data[18], 28);
    BOOST_CHECK_EQUAL(data[19], 29);
    BOOST_CHECK_EQUAL(data[20], 30);
    BOOST_CHECK_EQUAL(data[21], 31);
    BOOST_CHECK_EQUAL(data[22], 32);
    BOOST_CHECK_EQUAL(data[23], 33);

    // Last element, along second dimension
    // +----+----+----+----+----+  ----+
    // | 10 | 11 | 12 | 13 | 14 |   15 |
    // +----+----+----+----+----+  ----+
    // | 16 | 17 | 18 | 19 | 20 |   21 |
    // +----+----+----+----+----+  ----+
    // | 22 | 23 | 24 | 25 | 26 |   27 |
    // +----+----+----+----+----+  ----+
    // | 28 | 29 | 30 | 31 | 32 |   33 |
    // +----+----+----+----+----+  ----+
    data.erase(1, 5, 6);
    BOOST_CHECK_EQUAL(data.shape()[0], 4);
    BOOST_CHECK_EQUAL(data.shape()[1], 5);
    BOOST_CHECK_EQUAL(data[ 0], 10);
    BOOST_CHECK_EQUAL(data[ 1], 11);
    BOOST_CHECK_EQUAL(data[ 2], 12);
    BOOST_CHECK_EQUAL(data[ 3], 13);
    BOOST_CHECK_EQUAL(data[ 4], 14);
    BOOST_CHECK_EQUAL(data[ 5], 16);
    BOOST_CHECK_EQUAL(data[ 6], 17);
    BOOST_CHECK_EQUAL(data[ 7], 18);
    BOOST_CHECK_EQUAL(data[ 8], 19);
    BOOST_CHECK_EQUAL(data[ 9], 20);
    BOOST_CHECK_EQUAL(data[10], 22);
    BOOST_CHECK_EQUAL(data[11], 23);
    BOOST_CHECK_EQUAL(data[12], 24);
    BOOST_CHECK_EQUAL(data[13], 25);
    BOOST_CHECK_EQUAL(data[14], 26);
    BOOST_CHECK_EQUAL(data[15], 28);
    BOOST_CHECK_EQUAL(data[16], 29);
    BOOST_CHECK_EQUAL(data[17], 30);
    BOOST_CHECK_EQUAL(data[18], 31);
    BOOST_CHECK_EQUAL(data[19], 32);

    // Second column
    // +----+----+  ----  +----+----+
    // | 10 | 11 |   12   | 13 | 14 |
    // +----+----+  ----  +----+----+
    // | 16 | 17 |   18   | 19 | 20 |
    // +----+----+  ----  +----+----+
    // | 22 | 23 |   24   | 25 | 26 |
    // +----+----+  ----  +----+----+
    // | 28 | 29 |   30   | 31 | 32 |
    // +----+----+  ----  +----+----+
    data.erase(1, 2, 3);
    BOOST_CHECK_EQUAL(data.shape()[0], 4);
    BOOST_CHECK_EQUAL(data.shape()[1], 4);
    BOOST_CHECK_EQUAL(data[ 0], 10);
    BOOST_CHECK_EQUAL(data[ 1], 11);
    BOOST_CHECK_EQUAL(data[ 2], 13);
    BOOST_CHECK_EQUAL(data[ 3], 14);
    BOOST_CHECK_EQUAL(data[ 4], 16);
    BOOST_CHECK_EQUAL(data[ 5], 17);
    BOOST_CHECK_EQUAL(data[ 6], 19);
    BOOST_CHECK_EQUAL(data[ 7], 20);
    BOOST_CHECK_EQUAL(data[ 8], 22);
    BOOST_CHECK_EQUAL(data[ 9], 23);
    BOOST_CHECK_EQUAL(data[10], 25);
    BOOST_CHECK_EQUAL(data[11], 26);
    BOOST_CHECK_EQUAL(data[12], 28);
    BOOST_CHECK_EQUAL(data[13], 29);
    BOOST_CHECK_EQUAL(data[14], 31);
    BOOST_CHECK_EQUAL(data[15], 32);

    // Third row
    // +----+----+----+----+
    // | 10 | 11 | 13 | 14 |
    // +----+----+----+----+
    // | 16 | 17 | 19 | 20 |
    // +----+----+----+----+
    //
    // | 22 | 23 | 25 | 26 |
    //
    // +----+----+----+----+
    // | 28 | 29 | 31 | 32 |
    // +----+----+----+----+
    data.erase(0, 2, 3);
    BOOST_CHECK_EQUAL(data.shape()[0], 3);
    BOOST_CHECK_EQUAL(data.shape()[1], 4);
    BOOST_CHECK_EQUAL(data[ 0], 10);
    BOOST_CHECK_EQUAL(data[ 1], 11);
    BOOST_CHECK_EQUAL(data[ 2], 13);
    BOOST_CHECK_EQUAL(data[ 3], 14);
    BOOST_CHECK_EQUAL(data[ 4], 16);
    BOOST_CHECK_EQUAL(data[ 5], 17);
    BOOST_CHECK_EQUAL(data[ 6], 19);
    BOOST_CHECK_EQUAL(data[ 7], 20);
    BOOST_CHECK_EQUAL(data[ 8], 28);
    BOOST_CHECK_EQUAL(data[ 9], 29);
    BOOST_CHECK_EQUAL(data[10], 31);
    BOOST_CHECK_EQUAL(data[11], 32);

    // All the rest
    data.erase(1, 0, 4);
    BOOST_CHECK_EQUAL(data.nr_elements(), 0);
}


BOOST_AUTO_TEST_CASE(erase_3d)
{
    lue::Count const nr_elements0{2};
    lue::Count const nr_elements1{3};
    lue::Count const nr_elements2{4};
    Shape<3> const shape{nr_elements0, nr_elements1, nr_elements2};

    // +----+----+----+----+
    // | 10 | 11 | 12 | 13 |
    // +----+----+----+----+
    // | 14 | 15 | 16 | 17 |
    // +----+----+----+----+
    // | 18 | 19 | 20 | 21 |
    // +----+----+----+----+

    // +----+----+----+----+
    // | 22 | 23 | 24 | 25 |
    // +----+----+----+----+
    // | 26 | 27 | 28 | 29 |
    // +----+----+----+----+
    // | 30 | 31 | 32 | 33 |
    // +----+----+----+----+
    Data<3> data{shape};
    std::iota(data.begin(), data.end(), 10);
    BOOST_CHECK_EQUAL(data.nr_elements(), 24);
    BOOST_CHECK_EQUAL(data.shape()[0], 2);
    BOOST_CHECK_EQUAL(data.shape()[1], 3);
    BOOST_CHECK_EQUAL(data.shape()[2], 4);
    BOOST_CHECK_EQUAL(data[ 0], 10);
    BOOST_CHECK_EQUAL(data[23], 33);

    // Remove a slab from the inner part of the cube
    // +----+  ----+----  +----+
    // | 10 |   11 | 12   | 13 |
    // +----+  ----+----  +----+
    // | 14 |   15 | 16   | 17 |
    // +----+  ----+----  +----+
    // | 18 |   19 | 20   | 21 |
    // +----+  ----+----  +----+

    // +----+  ----+----  +----+
    // | 22 |   23 | 24   | 25 |
    // +----+  ----+----  +----+
    // | 26 |   27 | 28   | 29 |
    // +----+  ----+----  +----+
    // | 30 |   31 | 32   | 33 |
    // +----+  ----+----  +----+
    data.erase(2, 1, 3);
    BOOST_CHECK_EQUAL(data.shape()[0], 2);
    BOOST_CHECK_EQUAL(data.shape()[1], 3);
    BOOST_CHECK_EQUAL(data.shape()[2], 2);
    BOOST_CHECK_EQUAL(data[ 0], 10);
    BOOST_CHECK_EQUAL(data[ 1], 13);
    BOOST_CHECK_EQUAL(data[ 2], 14);
    BOOST_CHECK_EQUAL(data[ 3], 17);
    BOOST_CHECK_EQUAL(data[ 4], 18);
    BOOST_CHECK_EQUAL(data[ 5], 21);
    BOOST_CHECK_EQUAL(data[ 6], 22);
    BOOST_CHECK_EQUAL(data[ 7], 25);
    BOOST_CHECK_EQUAL(data[ 8], 26);
    BOOST_CHECK_EQUAL(data[ 9], 29);
    BOOST_CHECK_EQUAL(data[10], 30);
    BOOST_CHECK_EQUAL(data[11], 33);

    // Remove a slab from the inner part of the cube
    // +----+----+
    // | 10 | 13 |
    // +----+----+
    //
    // | 14 | 17 |
    //
    // +----+----+
    // | 18 | 21 |
    // +----+----+

    // +----+----+
    // | 22 | 25 |
    // +----+----+
    //
    // | 26 | 29 |
    //
    // +----+----+
    // | 30 | 33 |
    // +----+----+
    data.erase(1, 1, 2);
    BOOST_CHECK_EQUAL(data.shape()[0], 2);
    BOOST_CHECK_EQUAL(data.shape()[1], 2);
    BOOST_CHECK_EQUAL(data.shape()[2], 2);
    BOOST_CHECK_EQUAL(data[ 0], 10);
    BOOST_CHECK_EQUAL(data[ 1], 13);
    BOOST_CHECK_EQUAL(data[ 2], 18);
    BOOST_CHECK_EQUAL(data[ 3], 21);
    BOOST_CHECK_EQUAL(data[ 4], 22);
    BOOST_CHECK_EQUAL(data[ 5], 25);
    BOOST_CHECK_EQUAL(data[ 6], 30);
    BOOST_CHECK_EQUAL(data[ 7], 33);

    // +----+----+
    // | 22 | 25 |
    // +----+----+
    // | 30 | 33 |
    // +----+----+
    data.erase(0, 0, 1);
    BOOST_CHECK_EQUAL(data.shape()[0], 1);
    BOOST_CHECK_EQUAL(data.shape()[1], 2);
    BOOST_CHECK_EQUAL(data.shape()[2], 2);
    BOOST_CHECK_EQUAL(data[ 0], 22);
    BOOST_CHECK_EQUAL(data[ 1], 25);
    BOOST_CHECK_EQUAL(data[ 2], 30);
    BOOST_CHECK_EQUAL(data[ 3], 33);
}
