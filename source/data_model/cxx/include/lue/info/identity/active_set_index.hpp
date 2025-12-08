#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue::data_model {

    /*!
        @brief      Per location in time, an index of first object in active set
        @todo       Use private inheritance to hide the base class' API?

        In the ActiveSetIndex dataset, indices are stored of the first object of each active set. The ID
        of the active objects themselves can be stored using the ActiveID class.

        Tracking the indices of active sets is needed in combination with the following array kinds:

        - same_shape::constant_shape::Value
        - same_shape::variable_shape::Value
        - different_shape::constant_shape::Value
    */
    class LUE_DATA_MODEL_EXPORT ActiveSetIndex: public same_shape::constant_shape::Value
    {

        public:

            explicit ActiveSetIndex(hdf5::Group const& parent);

            explicit ActiveSetIndex(same_shape::constant_shape::Value&& value);

            ActiveSetIndex(ActiveSetIndex const& other) = default;

            ActiveSetIndex(ActiveSetIndex&& other) = default;

            ~ActiveSetIndex() override = default;

            auto operator=(ActiveSetIndex const& other) -> ActiveSetIndex& = default;

            auto operator=(ActiveSetIndex&& other) -> ActiveSetIndex& = default;

            auto nr_indices() const -> Count;

        private:
    };


    auto create_active_set_index(hdf5::Group& parent) -> ActiveSetIndex;

}  // namespace lue::data_model
