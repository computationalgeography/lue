#pragma once
#include "lue/object/dataset.hpp"
#include <memory>


namespace lue::data_model {

    class LUE_DATA_MODEL_EXPORT DatasetFixture
    {

        public:

            DatasetFixture();

            DatasetFixture(DatasetFixture const& other) = delete;

            DatasetFixture(DatasetFixture&& other) = delete;

            ~DatasetFixture();

            auto operator=(DatasetFixture const& other) -> DatasetFixture& = delete;

            auto operator=(DatasetFixture&& other) -> DatasetFixture& = delete;

            auto pathname() const -> std::string const&;

            auto dataset() -> Dataset&;

            void keep_dataset_upon_destruction();

        private:

            std::string const _pathname;

            bool _remove_dataset_upon_destruction;

            std::unique_ptr<Dataset> _dataset;
    };

}  // namespace lue::data_model
