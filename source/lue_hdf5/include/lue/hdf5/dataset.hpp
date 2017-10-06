#pragma once
#include "lue/hdf5/dataspace.hpp"
#include "lue/hdf5/datatype.hpp"
#include "lue/hdf5/hyperslab.hpp"
#include "lue/hdf5/property_list.hpp"
#include "lue/configure.hpp"


namespace lue {
namespace hdf5 {

bool               dataset_exists      (Identifier const& location,
                                        std::string const& name);


/*!
    @brief      Class representing an HDF5 dataset
*/
class Dataset
{

public:

    class CreationPropertyList:
        public PropertyList
    {

    public:

                   CreationPropertyList();

        void       set_chunk           (Shape const& chunk);

    };


    class TransferPropertyList:
        public PropertyList
    {

    public:

                   TransferPropertyList();

#ifdef HDF5_IS_PARALLEL
        void       set_transfer_mode   (H5FD_mpio_xfer_t const xfer_mode);
#endif

    };


                   Dataset             (Identifier const& location,
                                        std::string const& name);

                   Dataset             (Identifier&& id);

                   Dataset             (Dataset const& other)=delete;

                   Dataset             (Dataset&& other)=default;

    virtual        ~Dataset            ()=default;

    Dataset&       operator=           (Dataset const& other)=delete;

    Dataset&       operator=           (Dataset&& other)=default;

    Identifier const& id               () const;

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
                                        Dataspace const& memory_dataspace,
                                        void* buffer) const;

    void           read                (Datatype const& datatype,
                                        Dataspace const& memory_dataspace,
                                        Hyperslab const& hyperslab,
                                        void* buffer) const;

    void           write               (Datatype const& datatype,
                                        void const* buffer) const;

    void           write               (Datatype const& datatype,
                                        Hyperslab const& hyperslab,
                                        void const* buffer) const;

    void           write               (Datatype const& datatype,
                                        Dataspace const& memory_dataspace,
                                        Hyperslab const& hyperslab,
                                        void const* buffer) const;

    void           fill                (Datatype const& datatype,
                                        Hyperslab const& hyperslab,
                                        void const* buffer) const;

private:

    //! Identifier of dataset
    Identifier     _id;

};


bool               dataset_exists      (Identifier const& location,
                                        std::string const& name);

Dataset            open_dataset        (Identifier const& location,
                                        std::string const& name);

Dataset            create_dataset      (Identifier const& identifier,
                                        std::string const& name,
                                        Datatype const& datatype,
                                        Dataspace const& dataspace,
                                        Dataset::CreationPropertyList const&
                                            creation_property_list);

} // namespace hdf5
} // namespace lue
