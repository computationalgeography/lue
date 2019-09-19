#define BOOST_TEST_MODULE lue framework algorithm kernel
#include <hpx/config.hpp>
#include <boost/test/unit_test.hpp>
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/test/stream.hpp"


BOOST_AUTO_TEST_CASE(kernel_bool_1d)
{
    using Weight = bool;
    std::size_t const rank = 1;
    using Kernel = lue::Kernel<Weight, rank>;
    using Shape = lue::ShapeT<Kernel>;

    std::size_t const radius = 0;
    Shape shape{2 * radius + 1};
    Kernel kernel{shape};

    BOOST_CHECK_EQUAL(lue::rank<Kernel>, rank);
    BOOST_CHECK_EQUAL(lue::shape(kernel), shape);
    BOOST_CHECK_EQUAL(kernel.radius(), radius);
    BOOST_CHECK_EQUAL(lue::nr_elements(kernel), 1u);

    std::int32_t sum = std::accumulate(kernel.begin(), kernel.end(), 0);
    BOOST_CHECK_EQUAL(sum, 0);
}


BOOST_AUTO_TEST_CASE(square_kernel_bool_1d)
{
    using Weight = bool;
    std::size_t const rank = 1;
    using Kernel = lue::Kernel<Weight, rank>;
    using Shape = lue::ShapeT<Kernel>;

    std::size_t const radius = 1;
    bool const value = true;

    Kernel kernel = lue::square_kernel<Weight, rank>(radius, value);

    Shape shape{2 * radius + 1};
    BOOST_CHECK_EQUAL(lue::shape(kernel), shape);
    BOOST_CHECK_EQUAL(kernel.radius(), radius);
    BOOST_CHECK_EQUAL(lue::nr_elements(kernel), 3u);

    std::int32_t sum = std::accumulate(kernel.begin(), kernel.end(), 0);
    BOOST_CHECK_EQUAL(sum, 3);

    BOOST_CHECK_EQUAL(kernel[0], true);
    BOOST_CHECK_EQUAL(kernel[1], true);
    BOOST_CHECK_EQUAL(kernel[2], true);
}


BOOST_AUTO_TEST_CASE(square_kernel_bool_2d)
{
    using Weight = bool;
    std::size_t const rank = 2;
    using Kernel = lue::Kernel<Weight, rank>;
    using Shape = lue::ShapeT<Kernel>;

    std::size_t const radius = 1;
    bool const value = true;

    Kernel kernel = lue::square_kernel<Weight, rank>(radius, value);

    Shape shape{2 * radius + 1, 2 * radius + 1};
    BOOST_CHECK_EQUAL(lue::shape(kernel), shape);
    BOOST_CHECK_EQUAL(kernel.radius(), radius);
    BOOST_CHECK_EQUAL(lue::nr_elements(kernel), 9u);

    BOOST_CHECK_EQUAL(kernel[0][0], true);
    BOOST_CHECK_EQUAL(kernel[0][1], true);
    BOOST_CHECK_EQUAL(kernel[0][2], true);
    BOOST_CHECK_EQUAL(kernel[1][0], true);
    BOOST_CHECK_EQUAL(kernel[1][1], true);
    BOOST_CHECK_EQUAL(kernel[1][2], true);
    BOOST_CHECK_EQUAL(kernel[2][0], true);
    BOOST_CHECK_EQUAL(kernel[2][1], true);
    BOOST_CHECK_EQUAL(kernel[2][2], true);
}
