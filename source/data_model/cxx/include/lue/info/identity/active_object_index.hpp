#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {
    namespace data_model {

        /*!
            @brief      An index into the value array per object in the active set
            @todo       Use private inheritance to hide base class' API?

            In the ActiveObjectIndex dataset, indices can be stored of objects
            that are active at a given location in time. Multiple sets of indices
            can be stored sequentially. The index of the index of the first active
            object in a set can be stored using the ActiveSetIndex class.

            Tracking the indices of the active objects is needed in combination
            with the folowing object array kinds:
            - different_shape::constant_shape::Value

            The indices stored are the indices into the value array of each
            active object.
        */
        class ActiveObjectIndex: public same_shape::constant_shape::Value
        {

            public:

                explicit ActiveObjectIndex(hdf5::Group const& parent);

                explicit ActiveObjectIndex(same_shape::constant_shape::Value&& value);

                ActiveObjectIndex(ActiveObjectIndex const&) = default;

                ActiveObjectIndex(ActiveObjectIndex&&) = default;

                ~ActiveObjectIndex() override = default;

                ActiveObjectIndex& operator=(ActiveObjectIndex const&) = default;

                ActiveObjectIndex& operator=(ActiveObjectIndex&&) = default;

                Count nr_indices() const;

            private:
        };


        ActiveObjectIndex create_active_object_index(hdf5::Group& parent);

    }  // namespace data_model
}  // namespace lue
