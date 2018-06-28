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

   Dataset&        dataset             ();

private:

    std::string const _pathname;

    std::unique_ptr<Dataset> _dataset;

};

}  // namespace test
}  // namespace lue
