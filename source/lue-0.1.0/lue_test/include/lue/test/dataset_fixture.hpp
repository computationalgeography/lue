#pragma once
#include <string>


namespace lue {
namespace test {

class DatasetFixture
{

public:

                   DatasetFixture      (std::string const& dataset_name,
                                bool remove_dataset_upon_destruction=true);

                   DatasetFixture      (DatasetFixture const&)=delete;

                   DatasetFixture      (DatasetFixture&&)=delete;

                   ~DatasetFixture     ();

   DatasetFixture& operator=           (DatasetFixture const&)=delete;

   DatasetFixture& operator=           (DatasetFixture&&)=delete;

private:

    void           remove_dataset_if_it_exists();

    std::string const _dataset_name;

    bool const     _remove_dataset_upon_destruction;

};

}  // namespace test
}  // namespace lue
