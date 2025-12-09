#include "lue/data_model/hl/dataset_view.hpp"
#include "lue/data_model/hl/export.hpp"


namespace lue::data_model {

    /*!
        @brief      Construct an instance
        @param      dataset Pointer to dataset to manage
        @warning    @a dataset must point to a dataset
    */
    template<typename DatasetPtr>
    // cppcheck-suppress uninitMemberVar
    DatasetView<DatasetPtr>::DatasetView(DatasetPtr dataset):

        _dataset{std::move(dataset)}

    {
        assert(_dataset);
    }


    /*!
        @brief      Return the managed dataset
    */
    template<typename DatasetPtr>
    auto DatasetView<DatasetPtr>::operator*() const -> Dataset const&
    {
        return *_dataset;
    }


    /*!
        @brief      Return the managed dataset
    */
    template<typename DatasetPtr>
    auto DatasetView<DatasetPtr>::operator*() -> Dataset&
    {
        return *_dataset;
    }


#define LUE_INSTANTIATE_DATASET_VIEW(DatasetPtr) template class DatasetView<DatasetPtr>;

    LUE_INSTANTIATE_DATASET_VIEW(Dataset*)
    LUE_INSTANTIATE_DATASET_VIEW(std::shared_ptr<Dataset>)

#undef LUE_INSTANTIATE_DATASET_VIEW

}  // namespace lue::data_model
