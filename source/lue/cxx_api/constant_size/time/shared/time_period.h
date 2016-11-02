#pragma once
#include "lue/cxx_api/constant_size/time/omnipresent/same_shape/item.h"


namespace lue {
namespace constant_size {
namespace time {
namespace shared {

/*!
    @ingroup    lue_cxx_api_group
*/
class TimePeriod:
    public omnipresent::same_shape::Item
{

public:

                   TimePeriod          (hdf5::Identifier const& location);

                   TimePeriod          (hdf5::Identifier const& location,
                                        hid_t const type_id);

                   TimePeriod          (omnipresent::same_shape::Item&&
                                            coordinates);

                   TimePeriod          (TimePeriod const& other)=delete;

                   TimePeriod          (TimePeriod&& other)=default;

                   ~TimePeriod         ()=default;

    TimePeriod&    operator=           (TimePeriod const& other)=delete;

    TimePeriod&    operator=           (TimePeriod&& other)=default;

private:


};


TimePeriod           create_time_period  (hdf5::Identifier const& location,
                                        hid_t const file_type_id,
                                        hid_t const memory_type_id);

}  // namespace shared
}  // namespace time
}  // namespace constant_size
}  // namespace lue
