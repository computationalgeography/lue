#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {

/*!
    @todo       Privately inherit to hide base class' API?

    - Zero or more locations in time per item
*/
class LocationInTime:
    public same_shape::constant_shape::Value
{

public:

                   LocationInTime      (hdf5::Group& parent);

                   LocationInTime      (LocationInTime const&)=delete;

                   LocationInTime      (LocationInTime&&)=default;

                   LocationInTime      (
                                    same_shape::constant_shape::Value&& value);

                   ~LocationInTime     ()=default;

    LocationInTime& operator=          (LocationInTime const&)=delete;

    LocationInTime& operator=          (LocationInTime&&)=default;

    Count          nr_locations        () const;

private:

};


LocationInTime     create_location_in_time(
                                        hdf5::Group& parent,
                                        hdf5::Shape const& value_shape);

}  // namespace lue
