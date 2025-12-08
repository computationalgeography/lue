#include "lue/info/identity/active_object_id.hpp"
#include "lue/core/tag.hpp"


namespace lue::data_model {

    ActiveObjectID::ActiveObjectID(hdf5::Group const& parent):

        same_shape::constant_shape::Value{parent, active_object_id_tag}

    {
    }


    ActiveObjectID::ActiveObjectID(same_shape::constant_shape::Value&& value):

        same_shape::constant_shape::Value{std::move(value)}

    {
    }


    auto ActiveObjectID::nr_ids() const -> Count
    {
        return nr_arrays();
    }


    auto create_active_object_id(hdf5::Group& parent) -> ActiveObjectID
    {
        same_shape::constant_shape::Value value{same_shape::constant_shape::create_value(
            parent, active_object_id_tag, hdf5::Datatype{H5T_NATIVE_HSIZE})};

        return ActiveObjectID{std::move(value)};
    }

}  // namespace lue::data_model
