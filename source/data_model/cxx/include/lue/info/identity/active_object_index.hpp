#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue::data_model {

    /*!
        @brief      An index into the value array per object in the active set
        @todo       Use private inheritance to hide base class' API?

        In the ActiveObjectIndex dataset, indices can be stored of objects that are active at a given
        location in time. Multiple sets of indices can be stored sequentially. The index of the index of
        the first active object in a set can be stored using the ActiveSetIndex class.

        Tracking the indices of the active objects is needed in combination with the following object
        array kinds:

        - different_shape::constant_shape::Value

        The indices stored are the indices into the value array of each active object.
    */
    class LUE_DATA_MODEL_EXPORT ActiveObjectIndex: public same_shape::constant_shape::Value
    {

        public:

            explicit ActiveObjectIndex(hdf5::Group const& parent);

            explicit ActiveObjectIndex(same_shape::constant_shape::Value&& value);

            ActiveObjectIndex(ActiveObjectIndex const& other) = default;

            ActiveObjectIndex(ActiveObjectIndex&& other) = default;

            ~ActiveObjectIndex() override = default;

            auto operator=(ActiveObjectIndex const& other) -> ActiveObjectIndex& = default;

            auto operator=(ActiveObjectIndex&& other) -> ActiveObjectIndex& = default;

            auto nr_indices() const -> Count;

        private:
    };


    auto create_active_object_index(hdf5::Group& parent) -> ActiveObjectIndex;

}  // namespace lue::data_model
