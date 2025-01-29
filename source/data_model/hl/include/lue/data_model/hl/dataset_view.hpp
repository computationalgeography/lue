#pragma once
#include "lue/data_model/hl/export.hpp"
#include "lue/object/dataset.hpp"
#include <memory>


namespace lue::data_model {

    /*!
        @brief      Class for objects managing a dataset with a specific kind of data in mind

        A dataset view provides an API to a dataset that is convenient for a limited set of data kinds.
        Within a concrete view's implementation, it can be assumed that no other kinds of data are stored
        than the one the view is responsible for, although this does not imply that no other kinds of data
        are stored in the dataset. A single dataset can be potentially managed by multiple views.
    */
    template<typename DatasetPtr>
    class LUE_DATA_MODEL_HL_EXPORT DatasetView
    {

        public:

            auto operator*() const -> Dataset const&;

            auto operator*() -> Dataset&;

            virtual ~DatasetView() = default;

        protected:

            explicit DatasetView(DatasetPtr dataset);

            DatasetView(DatasetView const&) = default;

            DatasetView(DatasetView&&) noexcept = default;

            auto operator=(DatasetView const&) -> DatasetView& = default;

            auto operator=(DatasetView&&) noexcept -> DatasetView& = default;

        private:

            //! A pointer to the dataset this view manages
            DatasetPtr _dataset;
    };

}  // namespace lue::data_model
