#include "lue/info/identity/active_object_index.hpp"
#include "lue/core/tag.hpp"


namespace lue {
    namespace data_model {

        ActiveObjectIndex::ActiveObjectIndex(hdf5::Group const& parent):

            same_shape::constant_shape::Value{parent, active_object_index_tag}

        {
        }


        ActiveObjectIndex::ActiveObjectIndex(same_shape::constant_shape::Value&& value):

            same_shape::constant_shape::Value{std::move(value)}

        {
        }


        Count ActiveObjectIndex::nr_indices() const
        {
            return nr_arrays();
        }


        ActiveObjectIndex create_active_object_index(hdf5::Group& parent)
        {
            same_shape::constant_shape::Value value{same_shape::constant_shape::create_value(
                parent, active_object_index_tag, hdf5::Datatype{H5T_NATIVE_HSIZE})};

            return ActiveObjectIndex{std::move(value)};
        }

    }  // namespace data_model
}  // namespace lue
