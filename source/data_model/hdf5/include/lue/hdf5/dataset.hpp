#pragma once
#include "lue/hdf5/configure.hpp"
#include "lue/hdf5/dataspace.hpp"
#include "lue/hdf5/datatype.hpp"
#include "lue/hdf5/group.hpp"
#include "lue/hdf5/hyperslab.hpp"
#include "lue/hdf5/property_list.hpp"


namespace lue {
namespace hdf5 {

/*!
    @brief      Class representing an HDF5 dataset
*/
class Dataset:
    public PrimaryDataObject
{

public:

    class CreationPropertyList:
        public PropertyList
    {

    public:

                   CreationPropertyList();

        void       set_chunk           (Shape const& chunk);

        void       set_fill_time       (H5D_fill_time_t fill_time);

    };


    class AccessPropertyList:
        public PropertyList
    {

    public:

                   AccessPropertyList();

    };


    class TransferPropertyList:
        public PropertyList
    {

    public:

                   TransferPropertyList();

#ifdef HDF5_IS_PARALLEL
        void       set_transfer_mode   (::H5FD_mpio_xfer_t xfer_mode);
#endif

    };


                   Dataset             (Group const& parent,
                                        std::string const& name,
                                        AccessPropertyList const& access_property_list=
                                            AccessPropertyList{});

    explicit       Dataset             (Identifier&& id);

                   Dataset             (Dataset const&)=default;

                   Dataset             (Dataset&&)=default;

                   ~Dataset            () override =default;

    Dataset&       operator=           (Dataset const&)=default;

    Dataset&       operator=           (Dataset&&)=default;

    Datatype       datatype            () const;

    Dataspace      dataspace           () const;

    Shape          shape               () const;

    void           resize              (Shape const& new_dimension_sizes);

    void           read                (Datatype const& datatype,
                                        void* buffer) const;

    void           read                (Datatype const& datatype,
                                        Hyperslab const& hyperslab,
                                        void* buffer) const;

    void           read                (Datatype const& datatype,
                                        Hyperslab const& hyperslab,
                                        TransferPropertyList const& transfer_property_list,
                                        void* buffer) const;

    void           read                (Datatype const& datatype,
                                        Dataspace const& memory_dataspace,
                                        void* buffer) const;

    void           read                (Datatype const& datatype,
                                        Dataspace const& memory_dataspace,
                                        Hyperslab const& hyperslab,
                                        void* buffer) const;

    void           read                (Datatype const& datatype,
                                        Dataspace const& memory_dataspace,
                                        Hyperslab const& hyperslab,
                                        TransferPropertyList const& transfer_property_list,
                                        void* buffer) const;

    void           write               (Datatype const& datatype,
                                        void const* buffer) const;

    void           write               (Datatype const& datatype,
                                        Hyperslab const& hyperslab,
                                        void const* buffer) const;

    void           write               (Datatype const& datatype,
                                        Hyperslab const& hyperslab,
                                        TransferPropertyList const& transfer_property_list,
                                        void const* buffer) const;

    void           write               (Datatype const& datatype,
                                        Dataspace const& memory_dataspace,
                                        void const* buffer) const;

    void           write               (Datatype const& datatype,
                                        Dataspace const& memory_dataspace,
                                        Hyperslab const& hyperslab,
                                        void const* buffer) const;

    void           write               (Datatype const& datatype,
                                        Dataspace const& memory_dataspace,
                                        Hyperslab const& hyperslab,
                                        TransferPropertyList const& transfer_property_list,
                                        void const* buffer) const;

    void           fill                (Datatype const& datatype,
                                        Hyperslab const& hyperslab,
                                        void const* buffer) const;

private:

};


bool               dataset_exists      (Identifier const& parent,
                                        std::string const& name);

Dataset            open_dataset        (Identifier& parent,
                                        std::string const& name,
                                        Dataset::AccessPropertyList const& access_property_list=
                                            Dataset::AccessPropertyList{});

Dataset            create_dataset      (Identifier& parent,
                                        std::string const& name,
                                        Datatype const& datatype,
                                        Dataspace const& dataspace,
                                        Dataset::CreationPropertyList const& creation_property_list);

} // namespace hdf5
} // namespace lue
