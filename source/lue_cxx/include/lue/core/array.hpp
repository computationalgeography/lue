#pragma once
#include "lue/hdf5.hpp"


namespace lue {

class Array:
    public hdf5::Dataset
{

public:

                   Array               (hdf5::Group const& parent,
                                        std::string const& name);

                   Array               (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Array               (hdf5::Dataset&& dataset,
                                        hdf5::Datatype const& memory_datatype);

                   Array               (hdf5::Identifier const& id,
                                        hdf5::Datatype const& memory_datatype);

                   Array               (Array const&)=delete;

                   Array               (Array&&)=default;

                   ~Array              ()=default;

    Array&         operator=           (Array const&)=delete;

    Array&         operator=           (Array&&)=default;

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

} // namespace lue
