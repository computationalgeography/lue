#pragma once
#include "lue/array/same_shape/value.hpp"


namespace lue {
namespace data_model {

/*!
    @brief      An ID per object

    An ObjectID instance can be used to store the IDs of the active objects
    when this collection of objects does not change through time.

    @sa         same_shape::Value, different_shape::Value
*/
class ObjectID:
    public same_shape::Value
{

public:

    explicit       ObjectID            (hdf5::Group const& parent);

    explicit       ObjectID            (same_shape::Value&& value);

                   ObjectID            (ObjectID const&)=default;

                   ObjectID            (ObjectID&&)=default;

                   ~ObjectID           () override =default;

    ObjectID&      operator=           (ObjectID const&)=default;

    ObjectID&      operator=           (ObjectID&&)=default;

    Count          nr_objects          () const;

private:

};


ObjectID                 create_object_id(
                                        hdf5::Group& parent);

}  // namespace data_model
}  // namespace lue
