#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {
    namespace data_model {

        /*!
            - Zero or more space boxes per item
            - Each space box has a unique location in space
        */
        class MobileSpaceBox: public same_shape::constant_shape::Value
        {

            public:

                explicit MobileSpaceBox(hdf5::Group const& parent);

                MobileSpaceBox(hdf5::Group const& parent, hdf5::Datatype const& memory_datatype);

                explicit MobileSpaceBox(same_shape::constant_shape::Value&& value);

                MobileSpaceBox(MobileSpaceBox const&) = default;

                MobileSpaceBox(MobileSpaceBox&&) = default;

                ~MobileSpaceBox() override = default;

                MobileSpaceBox& operator=(MobileSpaceBox const&) = default;

                MobileSpaceBox& operator=(MobileSpaceBox&&) = default;

                Count nr_boxes() const;

            private:
        };


        MobileSpaceBox create_mobile_space_box(
            hdf5::Group& parent, hdf5::Datatype const& memory_datatype, std::size_t rank);

        MobileSpaceBox create_mobile_space_box(
            hdf5::Group& parent,
            hdf5::Datatype const& file_datatype,
            hdf5::Datatype const& memory_datatype,
            std::size_t rank);

    }  // namespace data_model
}  // namespace lue
