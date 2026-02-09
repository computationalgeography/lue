#pragma once
#include "lue/info/time/location_in_time.hpp"


namespace lue::data_model {

    /*!
        - Zero or more time boxes per item
        - Each time box has a unique location in time
    */
    class LUE_DATA_MODEL_EXPORT TimeBox: public LocationInTime
    {

        public:

            explicit TimeBox(hdf5::Group const& parent);

            explicit TimeBox(LocationInTime&& value);

            TimeBox(TimeBox const& other) = default;

            TimeBox(TimeBox&& other) = default;

            ~TimeBox() override = default;

            auto operator=(TimeBox const& other) -> TimeBox& = default;

            auto operator=(TimeBox&& other) -> TimeBox& = default;

            auto nr_boxes() const -> Count;

        private:
    };


    auto create_time_box(hdf5::Group& parent) -> TimeBox;


    template<>
    class ValueTraits<TimeBox>
    {

        public:

            using Element = typename ValueTraits<LocationInTime>::Element;
    };

}  // namespace lue::data_model
