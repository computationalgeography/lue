#include "lue/info/identity/active_object_index.hpp"
#include "lue/core/tag.hpp"


namespace lue::data_model {

    ActiveObjectIndex::ActiveObjectIndex(hdf5::Group const& parent):

        same_shape::constant_shape::Value{parent, active_object_index_tag}

    {
    }


    ActiveObjectIndex::ActiveObjectIndex(same_shape::constant_shape::Value&& value):

        same_shape::constant_shape::Value{std::move(value)}

    {
    }


    auto ActiveObjectIndex::nr_indices() const -> Count
    {
        return nr_arrays();
    }


    auto create_active_object_index(hdf5::Group& parent) -> ActiveObjectIndex
    {
        same_shape::constant_shape::Value value{same_shape::constant_shape::create_value(
            parent, active_object_index_tag, hdf5::Datatype{H5T_NATIVE_HSIZE})};

        return ActiveObjectIndex{std::move(value)};
    }

}  // namespace lue::data_model
