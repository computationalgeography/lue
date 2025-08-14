#pragma once
#include "lue/framework/partitioned_raster/export.hpp"
#include "lue/overload.hpp"
#include "lue/variant_wrapper.hpp"
#include <boost/serialization/strong_typedef.hpp>
#include <cstdint>
#include <string>


namespace lue {

    //! Type for the well-known text representation of a CRS
    BOOST_STRONG_TYPEDEF(std::string, WKT)

    //! Type for the PROJ.4 text representation of a CRS
    BOOST_STRONG_TYPEDEF(std::string, PROJ4)

    //! Type for representing European Petroleum Survey Group codes
    enum class EPSG : std::int16_t {};

    static EPSG const Cartesian2D{1024};

    // TODO: Add type to CRS, once needed: using PROJJSON = nlohmann::json;

    ////! Type for representing Spatial Reference System IDs
    // using SRID = EPSG;

    // //! Type for representing coordinate reference systems
    // using CRS = SRID;
    // using CRS = EPSG;

    //! Type for representing coordinate reference systems
    class LUE_PARTITIONED_RASTER_EXPORT CRS: public VariantWrapper<EPSG, PROJ4, WKT>
    {

        private:

            using Base = VariantWrapper;

        public:

            using VariantWrapper::VariantWrapper;


            enum class Type : std::uint8_t {
                Geodetic,
                Projected,
                Vertical,
                Engineering,
                Image,
            };


            CRS():

                Base(Cartesian2D)

            {
            }


            auto is_epsg() const -> bool
            {
                return std::holds_alternative<EPSG>(variant());
            }


            auto is_proj4() const -> bool
            {
                return std::holds_alternative<PROJ4>(variant());
            }


            auto is_wkt() const -> bool
            {
                return std::holds_alternative<WKT>(variant());
            }


            auto as_epsg() const -> EPSG const&
            {
                return std::get<EPSG>(variant());
            }


            auto as_proj4() const -> PROJ4 const&
            {
                return std::get<PROJ4>(variant());
            }


            auto as_wkt() const -> WKT const&
            {
                return std::get<WKT>(variant());
            }


            auto operator==(EPSG const& epsg) const -> bool
            {
                return is_epsg() && as_epsg() == epsg;
            }


            auto operator==(PROJ4 const& proj4) const -> bool
            {
                return is_proj4() && as_proj4() == proj4;
            }


            auto operator==(WKT const& wkt) const -> bool
            {
                return is_wkt() && as_wkt() == wkt;
            }

            auto type() const -> Type;
    };


    inline auto to_string(EPSG const& epsg) -> std::string
    {
        // TODO: C++23: std::to_underlying(epsg);
        return "EPSG:" + std::to_string(static_cast<std::int16_t>(epsg));
    }


    inline auto to_string(PROJ4 const& proj4) -> std::string
    {
        return proj4;
    }


    inline auto to_string(WKT const& wkt) -> std::string
    {
        return wkt;
    }


    inline auto to_string(CRS const& crs) -> std::string
    {
        return std::visit(
            overload{[](auto const& crs) -> std::string { return to_string(crs); }}, crs.variant());
    }


    namespace detail {

        /*!
            @brief      Overload called in case of unsupported combination of arguments

            We don't compare EPSG with WKT representations. Equality is defined here as "having the same
            type and value". Whether two representations with different types represent the same CRS is a
            different matter.
        */
        inline auto equal([[maybe_unused]] auto const& lhs, [[maybe_unused]] auto const& rhs) -> bool
        {
            return false;
        }


        inline auto equal(EPSG const& lhs, EPSG const& rhs) -> bool
        {
            return lhs == rhs;
        }

        inline auto equal(PROJ4 const& lhs, PROJ4 const& rhs) -> bool
        {
            return lhs == rhs;
        }

        inline auto equal(WKT const& lhs, WKT const& rhs) -> bool

        {
            return lhs == rhs;
        }

    }  // namespace detail


    inline auto operator==(CRS const& lhs, CRS const& rhs) -> bool
    {
        return std::visit(
            overload{[](auto const& lhs, auto const& rhs) -> bool { return detail::equal(lhs, rhs); }},
            lhs.variant(),
            rhs.variant());
    }

}  // namespace lue
