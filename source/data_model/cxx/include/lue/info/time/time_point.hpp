#pragma once
#include "lue/info/time/location_in_time.hpp"


namespace lue {
    namespace data_model {

        /*!
            - Zero or more time points per item
            - Each time point has a unique location in time
        */
        class TimePoint: public LocationInTime
        {

            public:

                explicit TimePoint(hdf5::Group const& parent);

                explicit TimePoint(LocationInTime&& value);

                TimePoint(TimePoint const&) = default;

                TimePoint(TimePoint&&) = default;

                ~TimePoint() override = default;

                TimePoint& operator=(TimePoint const&) = default;

                TimePoint& operator=(TimePoint&&) = default;

                Count nr_points() const;

            private:
        };


        TimePoint create_time_point(hdf5::Group& parent);


        template<>
        class ValueTraits<TimePoint>
        {

            public:

                using Element = typename ValueTraits<LocationInTime>::Element;
        };

    }  // namespace data_model
}  // namespace lue
