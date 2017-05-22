#pragma once
#include "lue/hdf5/count.h"
#include "lue/hdf5/offset.h"
#include "lue/hdf5/shape.h"
#include "lue/hdf5/stride.h"


namespace lue {
namespace hdf5 {

/*!
    @brief      Class representing a hyperslab selection of a dataspace
*/
class Hyperslab
{

public:

                   Hyperslab           (Offset const& start,
                                        Stride const& stride,
                                        Count const& count);

                   Hyperslab           (Shape const& shape);

                   Hyperslab           (Hyperslab const& other)=default;

                   Hyperslab           (Hyperslab&& other)=default;

                   ~Hyperslab          ()=default;

    Hyperslab&     operator=           (Hyperslab const& other)=default;

    Hyperslab&     operator=           (Hyperslab&& other)=default;

    Offset const&  start               () const;

    Offset&        start               ();

    Stride const&  stride              () const;

    Stride&        stride              ();

    Count const&   count               () const;

    Count&         count               ();

private:

    //! Offset of the starting element
    Offset         _start;

    //! Amount of elements to move in each direction
    Stride         _stride;

    //! Number of block to select
    Count          _count;

};

} // namespace hdf5
} // namespace lue
