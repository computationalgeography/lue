#include "lue/test/dataset_fixture.hpp"
#include <cassert>


namespace lue {
namespace test {

DatasetFixture::DatasetFixture()

    : _pathname{"my_dataset.lue"},
      _remove_dataset_upon_destruction{true},
      _dataset{std::make_unique<Dataset>(create_dataset(_pathname))}

{
    assert(dataset_exists(_pathname));
}


DatasetFixture::~DatasetFixture()
{
    if(_remove_dataset_upon_destruction) {
        if(dataset_exists(_pathname)) {
            _dataset.reset();
            remove_dataset(_pathname);
        }

        assert(!dataset_exists(_pathname));
    }
}


Dataset& DatasetFixture::dataset()
{
    return *_dataset;
}


void DatasetFixture::keep_dataset_upon_destruction()
{
    _remove_dataset_upon_destruction = false;
}

}  // namespace test
}  // namespace lue
