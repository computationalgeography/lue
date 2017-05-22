#include "lue/hdf5/hyperslab.h"
#include <cassert>


namespace lue {
namespace hdf5 {

Hyperslab::Hyperslab(
    Offset const& start,
    Stride const& stride,
    Count const& count)

    : _start{start},
      _stride{stride},
      _count{count}

{
}


/*!
    @brief      Construct an instance based on a @a shape

    The instance represents the whole dataspace of the @a shape passed in:
    - The offset is initialized to 0 in all directions
    - The stride is initialized to 1 in all directions
    - The count is initialized to the values in @a shape
*/
Hyperslab::Hyperslab(
    Shape const& shape)

    : _start(shape.size(), 0),
      _stride(shape.size(), 1),
      _count(shape.begin(), shape.end())

{
}


Offset const& Hyperslab::start() const
{
    return _start;
}


Offset& Hyperslab::start()
{
    return _start;
}


Stride const& Hyperslab::stride() const
{
    return _stride;
}


Stride& Hyperslab::stride()
{
    return _stride;
}


Count const& Hyperslab::count() const
{
    return _count;
}


Count& Hyperslab::count()
{
    return _count;
}

} // namespace hdf5
} // namespace lue
