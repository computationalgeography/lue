#pragma once
#include "lue/info/time/location_in_time.hpp"


namespace lue::data_model {

    /*!
        - Zero or more time points per item
        - Each time point has a unique location in time
    */
    class LUE_DATA_MODEL_EXPORT TimePoint: public LocationInTime
    {

        public:

            explicit TimePoint(hdf5::Group const& parent);

            explicit TimePoint(LocationInTime&& value);

            TimePoint(TimePoint const& other) = default;

            TimePoint(TimePoint&& other) = default;

            ~TimePoint() override = default;

            auto operator=(TimePoint const& other) -> TimePoint& = default;

            auto operator=(TimePoint&& other) -> TimePoint& = default;

            auto nr_points() const -> Count;

        private:
    };


    auto create_time_point(hdf5::Group& parent) -> TimePoint;


    template<>
    class ValueTraits<TimePoint>
    {

        public:

            using Element = typename ValueTraits<LocationInTime>::Element;
    };

}  // namespace lue::data_model
