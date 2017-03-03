#pragma once
#include "lue/hdf5/count.h"
#include "lue/hdf5/dataspace.h"
#include "lue/hdf5/datatype.h"
#include "lue/hdf5/offset.h"
#include "lue/hdf5/shape.h"
#include "lue/hdf5/stride.h"


namespace lue {
namespace hdf5 {

bool               dataset_exists      (Identifier const& location,
                                        std::string const& name);


class Dataset
{

public:

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
                                        Offset const& start,
                                        Stride const& stride,
                                        Count const& count,
                                        void* buffer) const;

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
                                        hid_t const creation_property_list);

} // namespace hdf5
} // namespace lue
