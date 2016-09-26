#pragma once
#include "lue/cxx_api/hdf5/identifier.h"
#include "lue/cxx_api/define.h"
#include <vector>


namespace lue {
namespace hdf5 {

/*!
    @ingroup    lue_cxx_api_group
*/
class Dataspace
{

public:

                   Dataspace           (Identifier&& id);

                   Dataspace           (Dataspace const& other)=delete;

                   Dataspace           (Dataspace&& other);

                   ~Dataspace          ()=default;

    Dataspace&     operator=           (Dataspace const& other)=delete;

    Dataspace&     operator=           (Dataspace&& other);

    Identifier const& id               () const;

    int            nr_dimensions       () const;

    std::vector<hsize_t>
                   dimension_extents   () const;

private:

    Identifier     _id;

};


Dataspace          create_dataspace    (std::vector<extent_t> const&
                                            dimension_sizes,
                                        std::vector<extent_t> const&
                                            max_dimension_sizes);

} // namespace hdf5
} // namespace lue
