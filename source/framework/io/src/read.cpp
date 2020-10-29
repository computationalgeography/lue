#include "lue/framework/io/write.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {

template<
    typename Array,
    typename DatasetPtr>
Array read(
    Array const& /* array */,
    typename data_model::variable::RasterView<DatasetPtr>::Layer const& /* layer */,
    data_model::Index const /* idx */)
{

    // Create new array with partitions that become ready once their
    // data has been read

    lue_assert(false);

    return Array{};
}


#define INSTANTIATE_READ_1(Element, DatasetPtr)                            \
template PartitionedArray<Element, 2>                                      \
    read<PartitionedArray<Element, 2>, DatasetPtr>(                        \
        PartitionedArray<Element, 2> const& array,                         \
        data_model::variable::RasterView<DatasetPtr>::Layer const& layer,  \
        data_model::Index const idx);

#define INSTANTIATE_READ(Element)                                      \
    INSTANTIATE_READ_1(Element, data_model::Dataset*)                  \
    INSTANTIATE_READ_1(Element, std::shared_ptr<data_model::Dataset>)

INSTANTIATE_READ(bool)
INSTANTIATE_READ(uint8_t)
INSTANTIATE_READ(uint32_t)
INSTANTIATE_READ(uint64_t)
// INSTANTIATE_READ(int8_t)
INSTANTIATE_READ(int32_t)
INSTANTIATE_READ(int64_t)
INSTANTIATE_READ(float)
INSTANTIATE_READ(double)

#undef INSTANTIATE_READ
#undef INSTANTIATE_READ_1

}  // namespace lue
