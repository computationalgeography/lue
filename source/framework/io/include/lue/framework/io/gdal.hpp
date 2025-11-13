#pragma once
#include "lue/framework/core/hyperslab.hpp"
#include "lue/framework/io/export.hpp"
#include "lue/framework/partitioned_raster.hpp"


namespace lue {

    template<typename Element>
    auto from_gdal(std::string const& name, Shape<Count, 2> const& partition_shape)
        -> PartitionedRaster<Element>;

    template<typename Element>
    auto from_gdal(
        std::string const& name, Hyperslab<2> const& hyperslab, Shape<Count, 2> const& partition_shape)
        -> PartitionedRaster<Element>;

    template<typename Element>
    auto to_gdal(
        PartitionedArray<Element, 2> const& array,
        std::string const& name,
        std::string const& clone_name = "") -> hpx::future<void>;

    template<typename Element>
    auto to_gdal(PartitionedRaster<Element> const& raster, std::string const& name) -> hpx::future<void>;

}  // namespace lue
