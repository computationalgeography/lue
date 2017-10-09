#pragma once
#include "lue/hdf5/dataset.hpp"
#include "lue/hdf5/count.hpp"
#include "lue/hdf5/offset.hpp"
#include "lue/hdf5/shape.hpp"
#include "lue/hdf5/stride.hpp"


namespace lue {

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

    void           read                (void* buffer) const;

    void           read                (hdf5::Hyperslab const& hyperslab,
                                        void* buffer) const;

    void           read                (hdf5::Dataspace const& memory_dataspace,
                                        void* buffer) const;

    void           write               (void const* buffer);

    void           write               (hdf5::Hyperslab const& hyperslab,
                                        void const* buffer);

    void           write               (hdf5::Dataspace const& memory_dataspace,
                                        void const* buffer) const;

    void           write               (hdf5::Dataspace const&
                                            memory_dataspace,
                                        hdf5::Hyperslab const& hyperslab,
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
