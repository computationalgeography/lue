#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue::data_model {

    /*!
        - Zero or more space boxes per item
        - Each space box has a unique location in space
    */
    class LUE_DATA_MODEL_EXPORT MobileSpaceBox: public same_shape::constant_shape::Value
    {

        public:

            explicit MobileSpaceBox(hdf5::Group const& parent);

            MobileSpaceBox(hdf5::Group const& parent, hdf5::Datatype const& memory_datatype);

            explicit MobileSpaceBox(same_shape::constant_shape::Value&& value);

            MobileSpaceBox(MobileSpaceBox const& other) = default;

            MobileSpaceBox(MobileSpaceBox&& other) = default;

            ~MobileSpaceBox() override = default;

            auto operator=(MobileSpaceBox const& other) -> MobileSpaceBox& = default;

            auto operator=(MobileSpaceBox&& other) -> MobileSpaceBox& = default;

            auto nr_boxes() const -> Count;

        private:
    };


    auto create_mobile_space_box(hdf5::Group& parent, hdf5::Datatype const& memory_datatype, std::size_t rank)
        -> MobileSpaceBox;

    auto create_mobile_space_box(
        hdf5::Group& parent,
        hdf5::Datatype const& file_datatype,
        hdf5::Datatype const& memory_datatype,
        std::size_t rank) -> MobileSpaceBox;

}  // namespace lue::data_model
