#pragma once
#include "lue/array/same_shape/value.hpp"


namespace lue {
namespace info {

/*!
    @brief      An ID per object

    An ID instance can be used to store the IDs of the active objects
    when this collection of objects does not change through time.
*/
class ID:
    public same_shape::Value
{

public:

                   ID                  (hdf5::Group& parent);

                   ID                  (same_shape::Value&& value);

                   ID                  (ID const&)=delete;

                   ID                  (ID&&)=default;

                   ~ID                 ()=default;

    ID&            operator=           (ID const&)=delete;

    ID&            operator=           (ID&&)=default;

    Count          nr_objects          () const;

private:

};


ID                 create_id           (hdf5::Group& parent);

}  // namespace info
}  // namespace lue
