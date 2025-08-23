#define BOOST_TEST_MODULE lue framework partitioned_raster crs
#include "lue/framework/partitioned_raster/crs.hpp"
#include "lue/framework/test/stream.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    lue::CRS const crs{};
    BOOST_CHECK_EQUAL(crs, lue::Cartesian2D);
}


BOOST_AUTO_TEST_CASE(epsg)
{
    lue::EPSG const epsg{4326};
    lue::CRS const crs{epsg};
    BOOST_CHECK_EQUAL(crs, epsg);
    BOOST_CHECK_EQUAL(to_string(crs), "EPSG:4326");
    // TODO: BOOST_CHECK_EQUAL(crs.type(), lue::CRS::Type::Projected);
}


BOOST_AUTO_TEST_CASE(proj4)
{
    std::string proj4_as_string{"+proj=longlat +datum=WGS84 +no_defs +type=crs"};
    lue::PROJ4 const proj4{proj4_as_string};
    lue::CRS const crs{proj4};
    BOOST_CHECK_EQUAL(crs, proj4);
    BOOST_CHECK_EQUAL(to_string(crs), proj4_as_string);
    // TODO: BOOST_CHECK_EQUAL(crs.type(), lue::CRS::Type::Projected);
}


BOOST_AUTO_TEST_CASE(wkt)
{
    std::string wkt_as_string{
        R"""(
GEOGCRS["WGS 84",
    ENSEMBLE["World Geodetic System 1984 ensemble",
        MEMBER["World Geodetic System 1984 (Transit)"],
        MEMBER["World Geodetic System 1984 (G730)"],
        MEMBER["World Geodetic System 1984 (G873)"],
        MEMBER["World Geodetic System 1984 (G1150)"],
        MEMBER["World Geodetic System 1984 (G1674)"],
        MEMBER["World Geodetic System 1984 (G1762)"],
        MEMBER["World Geodetic System 1984 (G2139)"],
        MEMBER["World Geodetic System 1984 (G2296)"],
        ELLIPSOID["WGS 84",6378137,298.257223563,
            LENGTHUNIT["metre",1]],
        ENSEMBLEACCURACY[2.0]],
    PRIMEM["Greenwich",0,
        ANGLEUNIT["degree",0.0174532925199433]],
    CS[ellipsoidal,2],
        AXIS["geodetic latitude (Lat)",north,
            ORDER[1],
            ANGLEUNIT["degree",0.0174532925199433]],
        AXIS["geodetic longitude (Lon)",east,
            ORDER[2],
            ANGLEUNIT["degree",0.0174532925199433]],
    USAGE[
        SCOPE["Horizontal component of 3D system."],
        AREA["World."],
        BBOX[-90,-180,90,180]],
    ID["EPSG",4326]]
)"""};
    lue::WKT const wkt{wkt_as_string};
    lue::CRS const crs{wkt};
    BOOST_CHECK_EQUAL(crs, wkt);
    BOOST_CHECK_EQUAL(to_string(crs), wkt_as_string);
    // TODO: BOOST_CHECK_EQUAL(crs.type(), lue::CRS::Type::Projected);
}
