#pragma once
#include "lue/hdf5/count.hpp"
#include "lue/hdf5/offset.hpp"
#include "lue/hdf5/shape.hpp"
#include "lue/hdf5/stride.hpp"


namespace lue {
namespace hdf5 {

/*!
    @brief      Class representing a hyperslab selection of a dataspace
    @sa         [Reading from or writing to a subset of a dataset](
                https://support.hdfgroup.org/HDF5/Tutor/selectsimple.html)

    A hyperslab defines a subset of a dataspace.
*/
class Hyperslab
{

public:

                   Hyperslab           (Offset const& start,
                                        Stride const& stride,
                                        Count const& count);

                   Hyperslab           (Offset const& start,
                                        Count const& count);

                   Hyperslab           (Shape const& shape);

                   Hyperslab           (Hyperslab const&)=default;

                   Hyperslab           (Hyperslab&&)=default;

                   ~Hyperslab          ()=default;

    Hyperslab&     operator=           (Hyperslab const&)=default;

    Hyperslab&     operator=           (Hyperslab&&)=default;

    Offset const&  start               () const;

    Offset&        start               ();

    Stride const&  stride              () const;

    Stride&        stride              ();

    Count const&   count               () const;

    Count&         count               ();

    std::size_t    nr_dimensions       () const;

    bool           empty               () const;

    std::size_t    nr_elements         () const;

private:

    //! Offset of the starting element
    Offset         _start;

    //! Amount of elements to move in each direction
    Stride         _stride;

    //! Number of blocks to select
    Count          _count;

};

} // namespace hdf5
} // namespace lue
