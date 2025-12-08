#pragma once
#include "lue/array/same_shape/value.hpp"


namespace lue::data_model {

    /*!
        - One space box per item
        - Each space box has a unique location in space
    */
    class LUE_DATA_MODEL_EXPORT StationarySpaceBox: public same_shape::Value
    {

        public:

            explicit StationarySpaceBox(hdf5::Group const& parent);

            StationarySpaceBox(hdf5::Group const& parent, hdf5::Datatype const& memory_datatype);

            explicit StationarySpaceBox(same_shape::Value&& value);

            StationarySpaceBox(StationarySpaceBox const& other) = default;

            StationarySpaceBox(StationarySpaceBox&& other) = default;

            ~StationarySpaceBox() override = default;

            auto operator=(StationarySpaceBox const& other) -> StationarySpaceBox& = default;

            auto operator=(StationarySpaceBox&& other) -> StationarySpaceBox& = default;

            auto nr_boxes() const -> Count;

        private:
    };


    auto create_stationary_space_box(
        hdf5::Group& parent, hdf5::Datatype const& memory_datatype, std::size_t rank) -> StationarySpaceBox;

    auto create_stationary_space_box(
        hdf5::Group& parent,
        hdf5::Datatype const& file_datatype,
        hdf5::Datatype const& memory_datatype,
        std::size_t rank) -> StationarySpaceBox;

}  // namespace lue::data_model
