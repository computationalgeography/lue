#pragma once
#include "lue/hdf5/dataspace.hpp"
#include "lue/hdf5/datatype.hpp"
#include "lue/hdf5/hyperslab.hpp"
#include "lue/hdf5/property_list.hpp"


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

    void           resize              (Shape const& new_dimension_sizes);

    void           read                (Datatype const& datatype,
                                        void* buffer) const;

    void           read                (Datatype const& datatype,
                                        Offset const& start,
                                        Count const& count,
                                        void* buffer) const;

    void           read                (Datatype const& datatype,
                                        Hyperslab const& hyperslab,
                                        void* buffer) const;

    void           read                (Datatype const& datatype,
                                        Offset const& start,
                                        Stride const& stride,
                                        Count const& count,
                                        void* buffer) const;

    void           write               (Datatype const& datatype,
                                        void const* buffer) const;

    void           write               (Datatype const& datatype,
                                        Offset const& start,
                                        Stride const& stride,
                                        Count const& count,
                                        void const* buffer) const;

    void           write               (Datatype const& datatype,
                                        Dataspace const& memory_dataspace,
                                        Offset const& start,
                                        Stride const& stride,
                                        Count const& count,
                                        void const* buffer) const;

private:

    Identifier     _id;

};


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
