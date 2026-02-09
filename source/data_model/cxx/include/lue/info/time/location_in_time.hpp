#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"
#include "lue/array/value_traits.hpp"
#include "lue/core/time/duration.hpp"


namespace lue::data_model {

    /*!
        @todo       Privately inherit to hide base class' API?

        - Zero or more locations in time per item
    */
    class LUE_DATA_MODEL_EXPORT LocationInTime: public same_shape::constant_shape::Value
    {

        public:

            using Element = time::DurationCount;

            explicit LocationInTime(hdf5::Group const& parent);

            explicit LocationInTime(same_shape::constant_shape::Value&& value);

            LocationInTime(LocationInTime const& other) = default;

            LocationInTime(LocationInTime&& other) = default;

            ~LocationInTime() override = default;

            auto operator=(LocationInTime const& other) -> LocationInTime& = default;

            auto operator=(LocationInTime&& other) -> LocationInTime& = default;

            auto nr_locations() const -> Count;

        private:
    };


    auto create_location_in_time(hdf5::Group& parent, hdf5::Shape const& value_shape) -> LocationInTime;


    template<>
    class ValueTraits<LocationInTime>
    {

        public:

            using Element = LocationInTime::Element;
    };

}  // namespace lue::data_model
