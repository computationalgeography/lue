#pragma once
#include "lue/array/same_shape/value.hpp"


namespace lue {
    namespace data_model {

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

                StationarySpacePoint(StationarySpacePoint const&) = default;

                StationarySpacePoint(StationarySpacePoint&&) = default;

                ~StationarySpacePoint() override = default;

                StationarySpacePoint& operator=(StationarySpacePoint const&) = default;

                StationarySpacePoint& operator=(StationarySpacePoint&&) = default;

                Count nr_points() const;

            private:
        };


        StationarySpacePoint create_stationary_space_point(
            hdf5::Group& parent, hdf5::Datatype const& memory_datatype, std::size_t rank);

        StationarySpacePoint create_stationary_space_point(
            hdf5::Group& parent,
            hdf5::Datatype const& file_datatype,
            hdf5::Datatype const& memory_datatype,
            std::size_t rank);

    }  // namespace data_model
}  // namespace lue
