#pragma once
#include "lue/array/same_shape/value.hpp"


namespace lue::data_model {

    /*!
        @brief      An ID per object

        An ObjectID instance can be used to store the IDs of the active objects when this collection of
        objects does not change through time.

        @sa         same_shape::Value, different_shape::Value
    */
    class LUE_DATA_MODEL_EXPORT ObjectID: public same_shape::Value
    {

        public:

            explicit ObjectID(hdf5::Group const& parent);

            explicit ObjectID(same_shape::Value&& value);

            ObjectID(ObjectID const& other) = default;

            ObjectID(ObjectID&& other) = default;

            ~ObjectID() override = default;

            auto operator=(ObjectID const& other) -> ObjectID& = default;

            auto operator=(ObjectID&& other) -> ObjectID& = default;

            auto nr_objects() const -> Count;

        private:
    };


    auto create_object_id(hdf5::Group& parent) -> ObjectID;

}  // namespace lue::data_model
