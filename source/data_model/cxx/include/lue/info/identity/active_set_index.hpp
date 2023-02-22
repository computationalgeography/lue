#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {
    namespace data_model {

        /*!
            @brief      Per location in time, an index of first object in active set
            @todo       Use private inheritance to hide the base class' API?

            In the ActiveSetIndex dataset, indices are stored of the first
            object of each active set. The ID of the active objects themselves
            can be stored using the ActiveID class.

            Tracking the indices of active sets is needed in combination with
            the folowing array kinds:
            - same_shape::constant_shape::Value
            - same_shape::variable_shape::Value
            - different_shape::constant_shape::Value
        */
        class ActiveSetIndex: public same_shape::constant_shape::Value
        {

            public:

                explicit ActiveSetIndex(hdf5::Group const& parent);

                explicit ActiveSetIndex(same_shape::constant_shape::Value&& value);

                ActiveSetIndex(ActiveSetIndex const&) = default;

                ActiveSetIndex(ActiveSetIndex&&) = default;

                ~ActiveSetIndex() override = default;

                ActiveSetIndex& operator=(ActiveSetIndex const&) = default;

                ActiveSetIndex& operator=(ActiveSetIndex&&) = default;

                Count nr_indices() const;

            private:
        };


        ActiveSetIndex create_active_set_index(hdf5::Group& parent);

    }  // namespace data_model
}  // namespace lue
