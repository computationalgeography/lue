#include "lue/test/dataset_fixture.hpp"
#include <cassert>


namespace lue {
namespace test {

DatasetFixture::DatasetFixture()

    : _pathname{"my_dataset.lue"},
      _dataset{std::make_unique<Dataset>(create_dataset(_pathname))}

{
    assert(dataset_exists(_pathname));
}


DatasetFixture::~DatasetFixture()
{
    if(dataset_exists(_pathname)) {
        _dataset.reset();
        remove_dataset(_pathname);
    }

    assert(!dataset_exists(_pathname));
}


Dataset& DatasetFixture::dataset()
{
    return *_dataset;
}

}  // namespace test
}  // namespace lue
