#pragma once
#include "lue/array/same_shape/value.hpp"


namespace lue {
    namespace data_model {

        /*!
            - One space box per item
            - Each space box has a unique location in space
        */
        class StationarySpaceBox: public same_shape::Value
        {

            public:

                explicit StationarySpaceBox(hdf5::Group const& parent);

                StationarySpaceBox(hdf5::Group const& parent, hdf5::Datatype const& memory_datatype);

                explicit StationarySpaceBox(same_shape::Value&& value);

                StationarySpaceBox(StationarySpaceBox const&) = default;

                StationarySpaceBox(StationarySpaceBox&&) = default;

                ~StationarySpaceBox() override = default;

                StationarySpaceBox& operator=(StationarySpaceBox const&) = default;

                StationarySpaceBox& operator=(StationarySpaceBox&&) = default;

                Count nr_boxes() const;

            private:
        };


        StationarySpaceBox create_stationary_space_box(
            hdf5::Group& parent, hdf5::Datatype const& memory_datatype, std::size_t rank);

        StationarySpaceBox create_stationary_space_box(
            hdf5::Group& parent,
            hdf5::Datatype const& file_datatype,
            hdf5::Datatype const& memory_datatype,
            std::size_t rank);

    }  // namespace data_model
}  // namespace lue
