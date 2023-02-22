#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {
    namespace data_model {

        /*!
            - Zero or more space points per item
            - Each space point has a unique location in space
        */
        class MobileSpacePoint: public same_shape::constant_shape::Value
        {

            public:

                explicit MobileSpacePoint(hdf5::Group const& parent);

                MobileSpacePoint(hdf5::Group const& parent, hdf5::Datatype const& memory_datatype);

                explicit MobileSpacePoint(same_shape::constant_shape::Value&& value);

                MobileSpacePoint(MobileSpacePoint const&) = default;

                MobileSpacePoint(MobileSpacePoint&&) = default;

                ~MobileSpacePoint() override = default;

                MobileSpacePoint& operator=(MobileSpacePoint const&) = default;

                MobileSpacePoint& operator=(MobileSpacePoint&&) = default;

                Count nr_points() const;

            private:
        };


        MobileSpacePoint create_mobile_space_point(
            hdf5::Group& parent, hdf5::Datatype const& memory_datatype, std::size_t rank);

        MobileSpacePoint create_mobile_space_point(
            hdf5::Group& parent,
            hdf5::Datatype const& file_datatype,
            hdf5::Datatype const& memory_datatype,
            std::size_t rank);

    }  // namespace data_model
}  // namespace lue
