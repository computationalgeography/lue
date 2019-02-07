#define BOOST_TEST_MODULE lue framework core array_partition_data
#include <boost/test/unit_test.hpp>
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/test/stream.hpp"


namespace {

using Value = std::int32_t;
std::size_t const rank = 2;
using Data = lue::ArrayPartitionData<Value, rank>;
using Shape = Data::Shape;
using Values = typename Data::Values;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(default_construct)
{
    // Default initialization
    {
        Data data;

        Shape shape{};
        BOOST_CHECK_EQUAL(data.shape(), shape);

        Values values{};
        BOOST_CHECK_EQUAL(data.values(), values);
    }

    // Value intialization
    {
        Data data{};

        Shape shape{};
        BOOST_CHECK_EQUAL(data.shape(), shape);

        Values values{};
        BOOST_CHECK_EQUAL(data.values(), values);
    }
}


BOOST_AUTO_TEST_CASE(construct_initialized_definition)
{
    Shape shape{{30, 40}};
    Data data{shape};

    BOOST_CHECK_EQUAL(data.shape(), shape);

    Values values{shape};
    std::fill(values.data(), values.data() + values.num_elements(), 0);
    BOOST_CHECK_EQUAL(data.values(), values);
}


BOOST_AUTO_TEST_CASE(construct_initialized_definition_and_value)
{
    Shape shape{{30, 40}};
    Data data{shape, 5};

    BOOST_CHECK_EQUAL(data.shape(), shape);

    Values values{shape};
    std::fill(values.data(), values.data() + values.num_elements(), 5);
    BOOST_CHECK_EQUAL(data.values(), values);
}


BOOST_AUTO_TEST_CASE(assign)
{
    Data data{};

    Shape shape{{30, 40}};

    data = Data{shape, 5};

    BOOST_CHECK_EQUAL(data.shape(), shape);

    Values values{shape};
    std::fill(values.data(), values.data() + values.num_elements(), 5);
    BOOST_CHECK_EQUAL(data.values(), values);
}


BOOST_AUTO_TEST_CASE(scalar_array)
{
    std::size_t const rank = 0;
    using Data = lue::ArrayPartitionData<Value, rank>;
    using Shape = Data::Shape;
    using Values = typename Data::Values;

    Shape shape{};

    {
        Data data;
        BOOST_CHECK_EQUAL(data.shape(), shape);
        BOOST_CHECK_EQUAL(data.size(), 1);
    }

    {
        Data data{5};
        BOOST_CHECK_EQUAL(data.shape(), shape);
        BOOST_CHECK_EQUAL(data.size(), 1);

        Values values;
        values[0] = 5;
        BOOST_CHECK_EQUAL(data.values(), values);
    }
}
