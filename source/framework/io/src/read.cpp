#include "lue/framework/io/write.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {

template<
    typename Array>
Array read(
    Array const& /* array */,
    data_model::variable::RasterView::Layer const& /* layer */,
    data_model::Index const /* idx */)
{

    // Create new array with partitions that become ready once their
    // data has been read

    assert(false);

}


#define INSTANTIATE_READ(Element)                                          \
                                                                           \
template PartitionedArray<Element, 2> read<PartitionedArray<Element, 2>>(  \
    PartitionedArray<Element, 2> const& array,                             \
    data_model::variable::RasterView::Layer const& layer,                  \
    data_model::Index const idx);

INSTANTIATE_READ(int32_t)
INSTANTIATE_READ(double)

#undef INSTANTIATE_READ

}  // namespace lue
