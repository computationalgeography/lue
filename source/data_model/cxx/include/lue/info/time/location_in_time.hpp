#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"
#include "lue/array/value_traits.hpp"
#include "lue/core/time/duration.hpp"


namespace lue {
namespace data_model {

/*!
    @todo       Privately inherit to hide base class' API?

    - Zero or more locations in time per item
*/
class LocationInTime:
    public same_shape::constant_shape::Value
{

public:

    using Element = time::DurationCount;

    explicit       LocationInTime      (hdf5::Group const& parent);

    explicit       LocationInTime      (same_shape::constant_shape::Value&& value);

                   LocationInTime      (LocationInTime const&)=default;

                   LocationInTime      (LocationInTime&&)=default;

                   ~LocationInTime     () override =default;

    LocationInTime& operator=          (LocationInTime const&)=default;

    LocationInTime& operator=          (LocationInTime&&)=default;

    Count          nr_locations        () const;

private:

};


LocationInTime     create_location_in_time(
                                        hdf5::Group& parent,
                                        hdf5::Shape const& value_shape);


template<>
class ValueTraits<LocationInTime>
{

public:

    using Element = LocationInTime::Element;

};

}  // namespace data_model
}  // namespace lue
