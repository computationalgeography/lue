#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue::data_model {

    /*!
        - Zero or more space points per item
        - Each space point has a unique location in space
    */
    class LUE_DATA_MODEL_EXPORT MobileSpacePoint: public same_shape::constant_shape::Value
    {

        public:

            explicit MobileSpacePoint(hdf5::Group const& parent);

            MobileSpacePoint(hdf5::Group const& parent, hdf5::Datatype const& memory_datatype);

            explicit MobileSpacePoint(same_shape::constant_shape::Value&& value);

            MobileSpacePoint(MobileSpacePoint const& other) = default;

            MobileSpacePoint(MobileSpacePoint&& other) = default;

            ~MobileSpacePoint() override = default;

            auto operator=(MobileSpacePoint const& other) -> MobileSpacePoint& = default;

            auto operator=(MobileSpacePoint&& other) -> MobileSpacePoint& = default;

            auto nr_points() const -> Count;

        private:
    };


    auto create_mobile_space_point(
        hdf5::Group& parent, hdf5::Datatype const& memory_datatype, std::size_t rank) -> MobileSpacePoint;

    auto create_mobile_space_point(
        hdf5::Group& parent,
        hdf5::Datatype const& file_datatype,
        hdf5::Datatype const& memory_datatype,
        std::size_t rank) -> MobileSpacePoint;

}  // namespace lue::data_model
