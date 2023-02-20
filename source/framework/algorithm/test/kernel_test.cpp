#define BOOST_TEST_MODULE lue framework algorithm kernel
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/test/stream.hpp"
#include <hpx/config.hpp>
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(kernel_bool_1d)
{
    using Weight = bool;
    lue::Rank const rank = 1;
    using Kernel = lue::Kernel<Weight, rank>;
    using Shape = lue::ShapeT<Kernel>;

    lue::Radius const radius = 0;
    Shape shape{2 * radius + 1};
    Kernel kernel{shape};

    BOOST_CHECK_EQUAL(lue::rank<Kernel>, rank);
    BOOST_CHECK_EQUAL(lue::shape(kernel), shape);
    BOOST_CHECK_EQUAL(kernel.radius(), radius);
    BOOST_CHECK_EQUAL(lue::nr_elements(kernel), 1);

    std::int32_t sum = std::accumulate(kernel.begin(), kernel.end(), 0);
    BOOST_CHECK_EQUAL(sum, 0);
}


BOOST_AUTO_TEST_CASE(box_kernel_bool_1d)
{
    using Weight = bool;
    lue::Rank const rank = 1;
    using Kernel = lue::Kernel<Weight, rank>;
    using Shape = lue::ShapeT<Kernel>;

    lue::Radius const radius = 1;
    bool const value = true;

    Kernel kernel = lue::box_kernel<Weight, rank>(radius, value);

    Shape shape{2 * radius + 1};
    BOOST_CHECK_EQUAL(lue::shape(kernel), shape);
    BOOST_CHECK_EQUAL(kernel.radius(), radius);
    BOOST_CHECK_EQUAL(lue::nr_elements(kernel), 3);

    std::int32_t sum = std::accumulate(kernel.begin(), kernel.end(), 0);
    BOOST_CHECK_EQUAL(sum, 3);

    BOOST_CHECK_EQUAL(kernel[0], true);
    BOOST_CHECK_EQUAL(kernel[1], true);
    BOOST_CHECK_EQUAL(kernel[2], true);
}


BOOST_AUTO_TEST_CASE(box_kernel_bool_2d)
{
    using Weight = bool;
    lue::Rank const rank = 2;
    using Kernel = lue::Kernel<Weight, rank>;
    using Shape = lue::ShapeT<Kernel>;

    lue::Radius const radius = 1;
    bool const value = true;

    Kernel kernel = lue::box_kernel<Weight, rank>(radius, value);

    Shape shape{2 * radius + 1, 2 * radius + 1};
    BOOST_CHECK_EQUAL(lue::shape(kernel), shape);
    BOOST_CHECK_EQUAL(kernel.radius(), radius);
    BOOST_CHECK_EQUAL(lue::nr_elements(kernel), 9);

    BOOST_CHECK_EQUAL(kernel(0, 0), true);
    BOOST_CHECK_EQUAL(kernel(0, 1), true);
    BOOST_CHECK_EQUAL(kernel(0, 2), true);
    BOOST_CHECK_EQUAL(kernel(1, 0), true);
    BOOST_CHECK_EQUAL(kernel(1, 1), true);
    BOOST_CHECK_EQUAL(kernel(1, 2), true);
    BOOST_CHECK_EQUAL(kernel(2, 0), true);
    BOOST_CHECK_EQUAL(kernel(2, 1), true);
    BOOST_CHECK_EQUAL(kernel(2, 2), true);
}


BOOST_AUTO_TEST_CASE(circle_kernel_bool_1d)
{
    using Weight = bool;
    lue::Rank const rank = 1;
    using Kernel = lue::Kernel<Weight, rank>;
    using Shape = lue::ShapeT<Kernel>;

    lue::Radius const radius = 1;
    bool const value = true;

    Kernel kernel = lue::circle_kernel<Weight, rank>(radius, value);

    Shape shape{2 * radius + 1};
    BOOST_CHECK_EQUAL(lue::shape(kernel), shape);
    BOOST_CHECK_EQUAL(kernel.radius(), radius);
    BOOST_CHECK_EQUAL(lue::nr_elements(kernel), 3);

    std::int32_t sum = std::accumulate(kernel.begin(), kernel.end(), 0);
    BOOST_CHECK_EQUAL(sum, 3);

    BOOST_CHECK_EQUAL(kernel[0], true);
    BOOST_CHECK_EQUAL(kernel[1], true);
    BOOST_CHECK_EQUAL(kernel[2], true);
}


BOOST_AUTO_TEST_CASE(circle_kernel_bool_2d)
{
    using Weight = bool;
    lue::Rank const rank = 2;
    using Kernel = lue::Kernel<Weight, rank>;
    using Shape = lue::ShapeT<Kernel>;

    bool const value = true;

    {
        lue::Radius const radius = 1;
        Kernel kernel = lue::circle_kernel<Weight, rank>(radius, value);

        Shape shape{2 * radius + 1, 2 * radius + 1};
        BOOST_CHECK_EQUAL(lue::shape(kernel), shape);
        BOOST_CHECK_EQUAL(kernel.radius(), radius);
        BOOST_CHECK_EQUAL(lue::nr_elements(kernel), 9);

        BOOST_CHECK_EQUAL(kernel(0, 0), false);
        BOOST_CHECK_EQUAL(kernel(0, 1), true);
        BOOST_CHECK_EQUAL(kernel(0, 2), false);
        BOOST_CHECK_EQUAL(kernel(1, 0), true);
        BOOST_CHECK_EQUAL(kernel(1, 1), true);
        BOOST_CHECK_EQUAL(kernel(1, 2), true);
        BOOST_CHECK_EQUAL(kernel(2, 0), false);
        BOOST_CHECK_EQUAL(kernel(2, 1), true);
        BOOST_CHECK_EQUAL(kernel(2, 2), false);
    }

    {
        lue::Radius const radius = 2;
        Kernel kernel = lue::circle_kernel<Weight, rank>(radius, value);

        Shape shape{2 * radius + 1, 2 * radius + 1};
        BOOST_CHECK_EQUAL(lue::shape(kernel), shape);
        BOOST_CHECK_EQUAL(kernel.radius(), radius);
        BOOST_CHECK_EQUAL(lue::nr_elements(kernel), 25);

        BOOST_CHECK_EQUAL(kernel(0, 0), false);
        BOOST_CHECK_EQUAL(kernel(0, 1), false);
        BOOST_CHECK_EQUAL(kernel(0, 2), true);
        BOOST_CHECK_EQUAL(kernel(0, 3), false);
        BOOST_CHECK_EQUAL(kernel(0, 4), false);

        BOOST_CHECK_EQUAL(kernel(1, 0), false);
        BOOST_CHECK_EQUAL(kernel(1, 1), true);
        BOOST_CHECK_EQUAL(kernel(1, 2), true);
        BOOST_CHECK_EQUAL(kernel(1, 3), true);
        BOOST_CHECK_EQUAL(kernel(1, 4), false);

        BOOST_CHECK_EQUAL(kernel(2, 0), true);
        BOOST_CHECK_EQUAL(kernel(2, 1), true);
        BOOST_CHECK_EQUAL(kernel(2, 2), true);
        BOOST_CHECK_EQUAL(kernel(2, 3), true);
        BOOST_CHECK_EQUAL(kernel(2, 4), true);

        BOOST_CHECK_EQUAL(kernel(3, 0), false);
        BOOST_CHECK_EQUAL(kernel(3, 1), true);
        BOOST_CHECK_EQUAL(kernel(3, 2), true);
        BOOST_CHECK_EQUAL(kernel(3, 3), true);
        BOOST_CHECK_EQUAL(kernel(3, 4), false);

        BOOST_CHECK_EQUAL(kernel(4, 0), false);
        BOOST_CHECK_EQUAL(kernel(4, 1), false);
        BOOST_CHECK_EQUAL(kernel(4, 2), true);
        BOOST_CHECK_EQUAL(kernel(4, 3), false);
        BOOST_CHECK_EQUAL(kernel(4, 4), false);
    }
}
