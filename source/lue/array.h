#pragma once
#include "lue/hdf5/dataset.h"
#include "lue/hdf5/count.h"
#include "lue/hdf5/offset.h"
#include "lue/hdf5/shape.h"
#include "lue/hdf5/stride.h"


namespace lue {

// bool               array_exists        (hdf5::Identifier const& location,
//                                         std::string const& name);


class Array:
    public hdf5::Dataset
{

public:

                   Array               (hdf5::Dataset&& dataset,
                                        hdf5::Datatype const& memory_datatype);

                   Array               (hdf5::Identifier const& location,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Array               (Array const& other)=delete;

                   Array               (Array&& other)=default;

                   ~Array              ()=default;

    Array&         operator=           (Array const& other)=delete;

    Array&         operator=           (Array&& other)=default;

    hdf5::Datatype const&
                   memory_datatype     () const;

    hdf5::Datatype file_datatype       () const;

    hdf5::Shape    shape               () const;

    void           read                (hdf5::Offset const& start,
                                        hdf5::Count const& count,
                                        void* buffer) const;

    void           read                (hdf5::Hyperslab const& hyperslab,
                                        void* buffer) const;

    void           read                (hdf5::Offset const& start,
                                        hdf5::Stride const& stride,
                                        hdf5::Count const& count,
                                        void* buffer) const;

    // void           write               (extent_t const count,
    //                                     void const* buffer);

    // void           write               (std::vector<extent_t> const count,
    //                                     void const* buffer);

    void           write               (hdf5::Offset const& start,
                                        hdf5::Stride const& stride,
                                        hdf5::Count const& count,
                                        void const* buffer);

    void           write               (hdf5::Dataspace const&
                                            memory_dataspace,
                                        hdf5::Offset const& start,
                                        hdf5::Stride const& stride,
                                        hdf5::Count const& count,
                                        void const* buffer);

private:

    //! In-memory type-id.
    hdf5::Datatype _memory_datatype;

};


// Array              open_array          (hdf5::Identifier const& location,
//                                         std::string const& name);
// 
// Array              open_array          (hdf5::Identifier const& location,
//                                         std::string const& name,
//                                         hid_t const memory_datatype);

} // namespace lue
