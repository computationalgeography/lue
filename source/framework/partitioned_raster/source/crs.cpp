#include "lue/framework/partitioned_raster/crs.hpp"

// Move to proj?
#include <proj/crs.hpp>


namespace lue {

    namespace detail {

        auto type(
            [[maybe_unused]] osgeo::proj::io::DatabaseContextNNPtr& database_context,
            [[maybe_unused]] EPSG const& epsg) -> CRS::Type
        {
            // auto authority_factory = osgeo::proj::io::AuthorityFactory::create(database_context, "EPSG");
            // auto crs = authority_factory->createCoordinateReferenceSystem("4326" /* to_string(epsg) */);
            // auto
            // Check the C-API and see if we can create a simple to use C++ API out of it.

            return CRS::Type::Engineering;
        }

        auto type(
            [[maybe_unused]] osgeo::proj::io::DatabaseContextNNPtr& database_context,
            [[maybe_unused]] PROJ4 const& proj4) -> CRS::Type
        {
            return CRS::Type::Engineering;
        }

        auto type(
            [[maybe_unused]] osgeo::proj::io::DatabaseContextNNPtr& database_context,
            [[maybe_unused]] WKT const& wkt) -> CRS::Type

        {
            return CRS::Type::Engineering;
        }

    }  // namespace detail


    auto CRS::type() const -> CRS::Type
    {
        auto database_context = osgeo::proj::io::DatabaseContext::create();
        // osgeo::proj::datum{crs.datum()};

        return std::visit(
            overload{[&](auto const& crs) -> Type { return detail::type(database_context, crs); }},
            variant());
    }

}  // namespace lue
