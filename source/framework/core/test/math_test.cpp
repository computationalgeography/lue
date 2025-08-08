#define BOOST_TEST_MODULE lue framework core math
#include "lue/framework/core/math.hpp"
#include <hpx/config.hpp>
#include <boost/test/included/unit_test.hpp>


namespace detail {

    template<typename Input, typename Output>
    auto check_equal = [](Input const from_min,
                          Input const from_max,
                          Output const to_min,
                          Output const to_max,
                          Input const value_to_map,
                          Output const result_we_want)
    {
        BOOST_TEST_INFO("With value " << value_to_map);
        BOOST_CHECK_EQUAL(
            lue::map_to_range(from_min, from_max, to_min, to_max, value_to_map), result_we_want);
    };

}  // namespace detail


#define DEFINE_CHECK_EQUAL()                                                                                 \
    auto check_equal = [=](Input const value_to_map, Output const result_we_want)                            \
    {                                                                                                        \
        detail::check_equal<Input, Output>(                                                                  \
            from_min, from_max, to_min, to_max, value_to_map, result_we_want);                               \
    };


BOOST_AUTO_TEST_CASE(map_indices)
{
    using Input = std::size_t;
    using Output = std::size_t;

    // Equal input and output ranges
    {
        Input from_min = 0;
        Input from_max = 10;
        Output to_min = 0;
        Output to_max = 10;

        DEFINE_CHECK_EQUAL()

        check_equal(0, 0);
        check_equal(1, 1);
        check_equal(2, 2);
        check_equal(3, 3);
        check_equal(4, 4);
        check_equal(5, 5);
        check_equal(6, 6);
        check_equal(7, 7);
        check_equal(8, 8);
        check_equal(9, 9);
        check_equal(10, 10);
    }

    // Smaller input range than output range
    {
        Input from_min = 0;
        Input from_max = 4;
        Output to_min = 0;
        Output to_max = 9;

        DEFINE_CHECK_EQUAL()

        check_equal(0, 0);
        check_equal(1, 2);
        check_equal(2, 4);
        check_equal(3, 6);
        check_equal(4, 8);
    }

    // Larger input range than output range
    {
        Input from_min = 0;
        Input from_max = 9;
        Output to_min = 0;
        Output to_max = 4;

        DEFINE_CHECK_EQUAL()

        check_equal(0, 0);
        check_equal(1, 0);
        check_equal(2, 1);
        check_equal(3, 1);
        check_equal(4, 2);
        check_equal(5, 2);
        check_equal(6, 3);
        check_equal(7, 3);
        check_equal(8, 4);
        check_equal(9, 4);
    }

    // Input range with single element
    {
        Input from_min = 0;
        Input from_max = 0;
        Output to_min = 0;
        Output to_max = 4;

        DEFINE_CHECK_EQUAL()

        check_equal(0, 0);
    }

    // Output range with single element
    {
        Input from_min = 0;
        Input from_max = 9;
        Output to_min = 0;
        Output to_max = 0;

        DEFINE_CHECK_EQUAL()

        check_equal(0, 0);
        check_equal(5, 0);
        check_equal(9, 0);
    }
}


BOOST_AUTO_TEST_CASE(map_to_range_case_1)
{
    using Input = std::size_t;
    using Output = std::size_t;

    // 100 partitions → 3 localities
    Input from_min = 0;
    Input from_max = 99;
    Output to_min = 0;
    Output to_max = 2;

    DEFINE_CHECK_EQUAL()

    //  0 - 33: 0 (→ 34)
    // 33 - 66: 1 (→ 33)
    // 66 - 99: 2 (→ 33)
    check_equal(0, 0);
    check_equal(33, 0);
    check_equal(34, 1);
    check_equal(66, 1);
    check_equal(67, 2);
    check_equal(99, 2);
}

#undef DEFINE_CHECK_EQUAL
