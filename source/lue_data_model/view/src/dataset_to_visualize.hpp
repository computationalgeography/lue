#pragma once
#include "lue/object/dataset.hpp"
#include <string>


namespace lue {
namespace utility {

class DatasetToVisualize
{

public:

                   DatasetToVisualize  (std::string const& name);

    std::string const& name            () const;

    Dataset const& dataset             () const;

private:

    std::string const _name;

    Dataset        _dataset;

};

}  // namespace utility
}  // namespace lue
