#pragma once
#include "lue/hdf5.hpp"
#include <cstdint>
#include <vector>


namespace lue::data_model {

    using Index = hsize_t;  // unsigned long long by default

    using ID = hsize_t;
    using IDs = std::vector<ID>;

    using Rank = int;

    using Count = hsize_t;
    using Counts = std::vector<Count>;

    using Shapes = std::vector<hdf5::Shape>;


    enum class TimeDomainItemType : std::uint8_t {

        point,

        box,

        cell

    };


    /*!
        @brief      Space domain items can be stationary or mobile through
                    time
    */
    enum class Mobility : std::uint8_t {
        //! Space domain items stay in the same location for the whole time domain
        stationary,

        //! Space domain items move around through time
        mobile
    };


    enum class SpaceDomainItemType : std::uint8_t {

        point,

        box,

        // line,

        // region,

        // cell

    };


    enum class ShapePerObject : std::uint8_t { same, different };


    enum class ValueVariability : std::uint8_t { constant, variable };


    enum class ShapeVariability : std::uint8_t { constant, variable };


    enum class TimeDiscretization : std::uint8_t { regular_grid };


    enum class SpaceDiscretization : std::uint8_t { regular_grid };

}  // namespace lue::data_model
