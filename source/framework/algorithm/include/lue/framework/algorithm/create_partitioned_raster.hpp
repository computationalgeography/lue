#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/partitioned_raster.hpp"


namespace lue {

    template<typename Element>
    auto create_partitioned_raster(
        Shape<Count, 2> const& raster_shape, Shape<Count, 2> const& partition_shape, Element const fill_value)
        -> PartitionedRaster<Element>
    {
        return create_partitioned_array(raster_shape, partition_shape, fill_value);
    }

}  // namespace lue
