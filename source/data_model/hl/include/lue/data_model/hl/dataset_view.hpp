#pragma once
#include "lue/object/dataset.hpp"
#include <memory>


namespace lue {
namespace data_model {

using DatasetPtr = std::shared_ptr<Dataset>;

/*!
    @brief      Class for objects managing a dataset with a specific
                kind of data in mind

    A dataset view provides an API to a dataset that is convenient for
    a limited set of data kinds. Within a concrete view's implementation,
    it can be assumed that no other kinds of data are stored than the
    one the view is responsible for, although this does not imply that
    no other kinds of data are stored in the dataset. A single dataset
    can be potentially managed by multiple views.
*/
class DatasetView
{

public:

    Dataset const& operator*           () const;

    Dataset&       operator*           ();

protected:

                   DatasetView         (DatasetPtr dataset);

                   DatasetView         (DatasetView const&)=default;

                   DatasetView         (DatasetView&&)=default;

    virtual        ~DatasetView        ()=default;

    DatasetView&   operator=           (DatasetView const&)=default;

    DatasetView&   operator=           (DatasetView&&)=default;

private:

    //! A pointer to the dataset this view manages
    DatasetPtr     _dataset;

};

}  // namespace data_model
}  // namespace lue
