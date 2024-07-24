#define BOOST_TEST_MODULE lue framework algorithm multiply
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/equal_to.hpp"
#include "lue/framework/algorithm/default_policies/modulus.hpp"
#include "lue/framework/algorithm/value_policies/modulus.hpp"
#include "lue/framework/algorithm/value_policies/none.hpp"
#include "lue/framework/algorithm/value_policies/valid.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace detail {

    template<typename Element, std::size_t rank>
    void test_array()
    {
        using namespace lue::default_policies;

        using Array = lue::PartitionedArray<Element, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Element const fill_value1{15};
        Element const fill_value2{6};

        Array array1{lue::create_partitioned_array(array_shape, partition_shape, fill_value1)};
        Array array2{lue::create_partitioned_array(array_shape, partition_shape, fill_value2)};

        // Modulus two arrays with fixed values
        {
            auto modulus = array1 % array2;
            auto equal_to = modulus == fill_value1 % fill_value2;

            BOOST_CHECK(all(equal_to).get());
        }

        // Modulus scalar with array
        // array * scalar
        {
            auto modulus = array1 % fill_value1;
            auto equal_to = modulus == fill_value1 % fill_value1;

            BOOST_CHECK(all(equal_to).get());
        }

        // Modulus scalar with array
        // scalar * array
        {
            auto modulus = fill_value1 % array1;
            auto equal_to = modulus == fill_value1 % fill_value1;

            BOOST_CHECK(all(equal_to).get());
        }
        
        /*
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

            //using Element = double;
            //lue::Rank const rank{2};

            using Array = lue::PartitionedArray<double, 2>;
            using Shape = lue::ShapeT<Array>;

            Shape shape{nr_rows, nr_cols};

            double const nd{std::numeric_limits<double>::quiet_NaN()};

            auto elevation = lue::test::create_partitioned_array<Array>(
                shape,
                shape,
                {{
                    70,  70,  80,  nd,  120, 70,  70,  90,  nd,  nd,  70,  70,  100,
                    140, 280, 180, 160, 110, 160, 320, 510, 440, 300, 400, 480,
                }});


            using Value = std::int32_t;
            using Class = std::uint64_t;
            std::size_t const rank_1 = 2;

            using ValueArray = lue::PartitionedArray<Value, rank_1>;
            using ClassArray = lue::PartitionedArray<Class, rank_1>;
            using Shape = lue::ShapeT<ValueArray>;

            Shape const array_shape{{9, 9}};
            Shape const partition_shape{{3, 3}};

            // Class const cx{lue::policy::no_data_value<Class>};  // Class no-data
            Value const vx{lue::policy::no_data_value<Value>};  // Value no-data

            //  1  2  3 |  4  5  6 |  7  8  9
            // 10 11 12 | 13 14 15 | 16 17 18
            // 19 20 21 | 22 23 24 | 25 26 27
            // ---------+----------+---------
            // 28 29 30 | vx 32 33 | 34 35 36
            // 37 38 39 | 40 41 42 | 43 44 45
            // 46 47 48 | 49 50 vx | 52 53 54
            // ---------+----------+---------
            // vx vx vx | 58 59 60 | 61 62 63
            // vx vx vx | 67 68 69 | 70 71 72
            // vx vx vx | 76 77 78 | 79 80 81

            ValueArray value_array = lue::test::create_partitioned_array<ValueArray>(
                array_shape,
                partition_shape,
                {
                    {1, 2, 3, 10, 11, 12, 19, 20, 21},
                    {4, 5, 6, 13, 14, 15, 22, 23, 24},
                    {7, 8, 9, 16, 17, 18, 25, 26, 27},
                    {28, 29, 30, 37, 38, 39, 46, 47, 48},
                    {vx, 32, 33, 40, 41, 42, 49, 50, vx},
                    {34, 35, 36, 43, 44, 45, 52, 53, 54},
                    {vx, vx, vx, vx, vx, vx, vx, vx, vx},
                    {58, 59, 60, 67, 68, 69, 76, 77, 78},
                    {61, 62, 63, 70, 71, 72, 79, 80, 81},
                });
        }
        */

        // Modulus scalar two array with random integer values
        {
            using Shape = lue::ShapeT<Array>;

            Shape const array_shape_specific{{4, 4}};
            Shape const partition_shape_specific{{2, 2}};

            // Element const nd{lue::policy::no_data_value<Element>};

            Array array3 = lue::test::create_partitioned_array<Array>(
                array_shape_specific,
                partition_shape_specific,
                {
                    // clang-format off
            {908, 12, 200, 5},
            {-5, -10, 809, -1},
            {0, 0, 0, 0},
            {5, 5, 5, 5},
                    // clang-format on
                });

            Array array4 = lue::test::create_partitioned_array<Array>(
                array_shape_specific,
                partition_shape_specific,
                {
                    // clang-format off
            {2, 3, 6, 5},
            {-5, 10, -10, 8},
            {2, 2, 2, 2},
            {5, 5, 5, 5},
                    // clang-format on
                });

            Array array_we_want = lue::test::create_partitioned_array<Array>(
                array_shape_specific,
                partition_shape_specific,
                {
                    // clang-format off
            {908 % 2, 12 % 3, 200 % 6, 5 % 5},
            {-5 % -5, -10 % 10, 809 % -10, -1 % 8},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
                    // clang-format on
                });

            auto modulus = array3 % array4;

            // Check if the result matches the expected output
            lue::test::check_arrays_are_equal(modulus, array_we_want);
        }

        /*
        // Modulus scalar two array with random values
        {
        using Shape = lue::ShapeT<Array>;

        Shape const array_shape_specific{{9, 9}};
        Shape const partition_shape_specific{{3, 3}};

        Element const nd{lue::policy::no_data_value<Element>};

        Array array3 = lue::test::create_partitioned_array<Array>(
        array_shape_specific,
        partition_shape_specific,
        {
            // clang-format off
            {908, 12, 200, 5, 1, 2, 8, 10, 11},
            {-5, nd, nd, -3, -40, -50, -121, -45.89, 14},
            {0, 0, 0.000001, nd, -0.00001, 0.99999999, nd, 0.9999998, -0.9999999},

            {5, 5, 5, 5, 5, 5, 5, 5, 5},
            {5, 5, 5, 5, 5, 5, 5, 5, 5},
            {5, 5, 5, 5, 5, 5, 5, 5, 5},

            {5, 5, 5, 5, 5, 5, 5, 5, 5},
            {5, 5, 5, 5, 5, 5, 5, 5, 5},
            {5, 5, 5, 5, 5, 5, 5, 5, 5},
            // clang-format on
        });

        Array array4 = lue::test::create_partitioned_array<Array>(
        array_shape_specific,
        partition_shape_specific,
        {
            // clang-format off
            {2, 3, 6, 5, 1, 2, 8, 10, 11},
            {-5, 10, 10.7896, 8, 4, 6, -12, -13, -3},
            {3, -3, 0.000001, nd, 3, 0.99999999, nd, 0.9999998, -0.9999999},

            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},

            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            // clang-format on
        });



        Array array_we_want = lue::test::create_partitioned_array<Array>(
        array_shape_specific,
        partition_shape_specific,
        {
            // clang-format off
            {908 % 2, 12 % 3, 200 % 6, 5 % 5, 0 , 0, 0, 0, 0},
            {-5 % -5, nd, nd, -3 % 8, -40 % 4, -50 % 6, -121 % -12, -45.89 % -13, 14 %-3},
            {0, 0, nd, nd, nd, nd, nd, nd, nd},

            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},

            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            // clang-format on
        });

        auto modulus = array3 % array4;

        // Check if the result matches the expected output
        lue::test::check_arrays_are_equal(modulus, array_we_want);

        }
        */
    }

}  // namespace detail


#define TEST_CASE(rank, Element)                                                                             \
                                                                                                             \
    BOOST_AUTO_TEST_CASE(array_##rank##d_##Element)                                                          \
    {                                                                                                        \
        detail::test_array<Element, rank>();                                                                 \
    }

// TEST_CASE(1, int32_t)
TEST_CASE(2, int32_t)
// TEST_CASE(1, int64_t)
// TEST_CASE(2, int64_t)
// TEST_CASE(1, float)
// TEST_CASE(2, float)
// TEST_CASE(1, double)
// TEST_CASE(2, double)

#undef TEST_CASE
