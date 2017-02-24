#pragma once
#include "lue/hdf5/identifier.h"
#include "lue/hdf5/shape.h"


namespace lue {
namespace hdf5 {

class Dataspace
{

public:

                   Dataspace           (::H5S_class_t const type);

                   Dataspace           (hid_t const id);

                   Dataspace           (Identifier&& id);

                   Dataspace           (Dataspace const& other)=delete;

                   Dataspace           (Dataspace&& other);

                   ~Dataspace          ()=default;

    Dataspace&     operator=           (Dataspace const& other)=delete;

    Dataspace&     operator=           (Dataspace&& other);

    Identifier const& id               () const;

    int            nr_dimensions       () const;

    Shape          dimension_extents   () const;

private:

    Identifier     _id;

};


Dataspace          create_dataspace    (Shape const& dimension_sizes);

Dataspace          create_dataspace    (Shape const& dimension_sizes,
                                        Shape const& max_dimension_sizes);

} // namespace hdf5
} // namespace lue
