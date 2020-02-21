#include "lue/data_model/hl/dataset_view.hpp"


namespace lue {
namespace data_model {

/*!
    @brief      Construct an instance
    @param      dataset Pointer to dataset to manage
    @warning    @a dataset must point to a dataset
*/
DatasetView::DatasetView(
    DatasetPtr dataset):

    _dataset{std::move(dataset)}

{
    assert(_dataset);
}


/*!
    @brief      Return the managed dataset
*/
Dataset const& DatasetView::operator*() const
{
    return *_dataset;
}


/*!
    @brief      Return the managed dataset
*/
Dataset& DatasetView::operator*()
{
    return *_dataset;
}

}  // namespace data_model
}  // namespace lue
