#include "lue/data_model/hl/dataset_view.hpp"


namespace lue {
namespace data_model {

/*!
    @brief      Construct an instance
    @param      dataset Pointer to dataset to manage
    @warning    @a dataset must point to a dataset
*/
template<
    typename DatasetPtr>
// cppcheck-suppress uninitMemberVar
DatasetView<DatasetPtr>::DatasetView(
    DatasetPtr dataset):

    _dataset{std::move(dataset)}

{
    assert(_dataset);
}


/*!
    @brief      Return the managed dataset
*/
template<
    typename DatasetPtr>
Dataset const& DatasetView<DatasetPtr>::operator*() const
{
    return *_dataset;
}


/*!
    @brief      Return the managed dataset
*/
template<
    typename DatasetPtr>
Dataset& DatasetView<DatasetPtr>::operator*()
{
    return *_dataset;
}


#define INSTANTIATE_DATASET_VIEW(DatasetPtr)  \
template class DatasetView<DatasetPtr>;

INSTANTIATE_DATASET_VIEW(Dataset*)
INSTANTIATE_DATASET_VIEW(std::shared_ptr<Dataset>)

#undef INSTANTIATE_DATASET_VIEW

}  // namespace data_model
}  // namespace lue
