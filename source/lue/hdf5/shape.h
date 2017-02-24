#pragma once
#include <hdf5.h>
#include <vector>


namespace lue {
namespace hdf5 {

class Shape: public std::vector<hsize_t> {
    using std::vector<hsize_t>::vector;
};

}  // namespace hdf5
}  // namespace lue
