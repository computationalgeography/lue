#include "dataset_to_visualize.hpp"


namespace lue {
namespace utility {

DatasetToVisualize::DatasetToVisualize(
    std::string const& name):

    _name{name},
    _dataset{name}

{
}


std::string const& DatasetToVisualize::name() const
{
    return _name;
}


Dataset const& DatasetToVisualize::dataset() const
{
    return _dataset;
}

}  // namespace utility
}  // namespace lue
