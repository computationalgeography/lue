#include "lue/hdf5/hyperslab.hpp"
#include <algorithm>
#include <cassert>


namespace lue {
namespace hdf5 {

/*!
    @brief      Construct an instance based on a @a start, @a stride,
                and @a count

    The size of @a start, @a stride, and @a count must be equal to the rank
    of the dataset's dataspace.
*/
Hyperslab::Hyperslab(
    Offset const& start,
    Stride const& stride,
    Count const& count)

    : _start{start},
      _stride{stride},
      _count{count}

{
    assert(_start.size() == _stride.size());
    assert(_start.size() == _count.size());
}


/*!
    @brief      Construct an instance based on a @a shape

    The instance represents the whole dataspace of the @a shape passed in:
    - The offset is initialized to 0 in all directions
    - The stride is initialized to 1 in all directions
    - The count is initialized to the values in @a shape

    The size of @a shape must be equal to the rank of the dataset's
    dataspace.
*/
Hyperslab::Hyperslab(
    Shape const& shape)

    : _start(shape.size(), 0),
      _stride(shape.size(), 1),
      _count(shape.begin(), shape.end())

{
    assert(_start.size() == _stride.size());
    assert(_start.size() == _count.size());
}


/*!
    @brief      Return the offset in each dimension of the dataspace
                selection
*/
Offset const& Hyperslab::start() const
{
    return _start;
}


/*!
    @brief      Return the offset in each dimension of the dataspace
                selection
*/
Offset& Hyperslab::start()
{
    return _start;
}


/*!
    @brief      Return the stride of the blocks in each dimension of
                the dataspace selection
*/
Stride const& Hyperslab::stride() const
{
    return _stride;
}


/*!
    @brief      Return the stride of the blocks in each dimension of
                the dataspace selection
*/
Stride& Hyperslab::stride()
{
    return _stride;
}


/*!
    @brief      Return the number of blocks in each dimension of the
                dataspace selection
*/
Count const& Hyperslab::count() const
{
    return _count;
}


/*!
    @brief      Return the number of blocks in each dimension of the
                dataspace selection
*/
Count& Hyperslab::count()
{
    return _count;
}


/*!
    @brief      Return the number of dimensions of the dataspace selection
*/
std::size_t Hyperslab::nr_dimensions() const
{
    return _start.size();
}


/*!
    @brief      Return the dataspace selection is empty

    This happens in case the number of dimensions is zero or one of the
    counts or strides is 0.
*/
bool Hyperslab::empty() const
{
    return
        nr_dimensions() == 0 ||
        std::find(_count.begin(), _count.end(), 0) != _count.end() ||
        std::find(_stride.begin(), _stride.end(), 0) != _stride.end()
        ;
}


/*!
    @brief      Return the number of selected elements
*/
std::size_t Hyperslab::nr_elements() const
{
    std::size_t result = 0;

    if(!empty()) {
        result = 1;

        for(size_t i = 0; i < nr_dimensions(); ++i) {
            result *= (_stride[i] * _count[i]);
        }
    }

    return result;
}

} // namespace hdf5
} // namespace lue
