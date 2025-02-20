#pragma once
#include "lue/framework/core/hyperslab.hpp"
#include "lue/framework/io/export.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<typename Element>
    LUE_FRAMEWORK_IO_EXPORT auto from_gdal(std::string const& name, Shape<Count, 2> const& partition_shape)
        -> PartitionedArray<Element, 2>;

    template<typename Element>
    LUE_FRAMEWORK_IO_EXPORT auto from_gdal(
        std::string const& name,
        Hyperslab<2> const& hyperslab,
        Shape<Count, 2> const& partition_shape) -> PartitionedArray<Element, 2>;

    template<typename Element>
    LUE_FRAMEWORK_IO_EXPORT auto to_gdal(
        PartitionedArray<Element, 2> const& array,
        std::string const& name,
        std::string const& clone_name = "") -> hpx::future<void>;

}  // namespace lue
