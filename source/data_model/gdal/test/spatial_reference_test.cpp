#define BOOST_TEST_MODULE lue gdal spatial_reference
#include "lue/gdal/error.hpp"
#include "lue/gdal/spatial_reference.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(from_wkt)
{
    namespace lgd = lue::gdal;
    lgd::QuietErrorHandler error_handler{};

    BOOST_CHECK_THROW(lgd::SpatialReference::from_wkt("meh"), std::runtime_error);
}


BOOST_AUTO_TEST_CASE(from_epsg)
{
    namespace lgd = lue::gdal;
    lgd::QuietErrorHandler error_handler{};

    BOOST_CHECK_THROW(lgd::SpatialReference::from_epsg(0), std::runtime_error);
    BOOST_CHECK_NO_THROW(lgd::SpatialReference::from_epsg(4326));
}


BOOST_AUTO_TEST_CASE(projected)
{
    namespace lgd = lue::gdal;
    lgd::QuietErrorHandler error_handler{};

    BOOST_CHECK_THROW(lgd::SpatialReference::from_wkt("meh"), std::runtime_error);

    auto const srs = lgd::SpatialReference::from_epsg(4326);
    BOOST_CHECK(!srs.is_projected());
}
