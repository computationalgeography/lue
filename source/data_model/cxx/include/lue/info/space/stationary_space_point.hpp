#pragma once
#include "lue/array/same_shape/value.hpp"


namespace lue::data_model {

    /*!
        - One space point per item
        - Each space point has a unique location in space
    */
    class LUE_DATA_MODEL_EXPORT StationarySpacePoint: public same_shape::Value
    {

        public:

            explicit StationarySpacePoint(hdf5::Group const& parent);

            StationarySpacePoint(hdf5::Group const& parent, hdf5::Datatype const& memory_datatype);

            explicit StationarySpacePoint(same_shape::Value&& value);

            StationarySpacePoint(StationarySpacePoint const& other) = default;

            StationarySpacePoint(StationarySpacePoint&& other) = default;

            ~StationarySpacePoint() override = default;

            auto operator=(StationarySpacePoint const& other) -> StationarySpacePoint& = default;

            auto operator=(StationarySpacePoint&& other) -> StationarySpacePoint& = default;

            auto nr_points() const -> Count;

        private:
    };


    auto create_stationary_space_point(
        hdf5::Group& parent, hdf5::Datatype const& memory_datatype, std::size_t rank) -> StationarySpacePoint;

    auto create_stationary_space_point(
        hdf5::Group& parent,
        hdf5::Datatype const& file_datatype,
        hdf5::Datatype const& memory_datatype,
        std::size_t rank) -> StationarySpacePoint;

}  // namespace lue::data_model
