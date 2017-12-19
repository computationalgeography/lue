#include "lue/test/dataset_fixture.hpp"
#include "lue/dataset.hpp"
#include <iostream>


namespace lue {
namespace test {

DatasetFixture::DatasetFixture(
    std::string const& dataset_name,
    bool const remove_dataset_upon_destruction)

    : _dataset_name{dataset_name},
      _remove_dataset_upon_destruction{remove_dataset_upon_destruction}

{
    // Guarantees:
    // - Dataset does not exist after setup
    remove_dataset_if_it_exists();
}


DatasetFixture::~DatasetFixture()
{
    std::cout << _remove_dataset_upon_destruction << std::endl;
    if(_remove_dataset_upon_destruction) {
        // Guarantees:
        // - Dataset does not exist after teardown
        remove_dataset_if_it_exists();
    }
}


void DatasetFixture::remove_dataset_if_it_exists()
{
    if(lue::dataset_exists(_dataset_name)) {
        lue::remove_dataset(_dataset_name);
    }
}

}  // namespace test
}  // namespace lue
