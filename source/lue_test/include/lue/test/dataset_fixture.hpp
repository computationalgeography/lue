#pragma once
#include "lue/object/dataset.hpp"
#include <memory>


namespace lue {
namespace test {

class DatasetFixture
{

public:

                   DatasetFixture      ();

                   DatasetFixture      (DatasetFixture const&)=delete;

                   DatasetFixture      (DatasetFixture&&)=delete;

                   ~DatasetFixture     ();

   DatasetFixture& operator=           (DatasetFixture const&)=delete;

   DatasetFixture& operator=           (DatasetFixture&&)=delete;

   std::string const& pathname         () const;

   Dataset&        dataset             ();

   void            keep_dataset_upon_destruction();

private:

    std::string const _pathname;

    bool           _remove_dataset_upon_destruction;

    std::unique_ptr<Dataset> _dataset;

};

}  // namespace test
}  // namespace lue
