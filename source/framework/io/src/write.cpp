#include "lue/framework/io/write.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {

template<
    typename Array>
void write(
    Array const& /* array */,
    std::string const& /* pathname */)
{
    // A 2D array can be written to a raster dataset using GDAL

    int i = 5;
    i += 6;
}


template void write<PartitionedArray<std::int32_t, 2>>(
    PartitionedArray<std::int32_t, 2> const& array,
    std::string const& pathname);

}  // namespace lue
