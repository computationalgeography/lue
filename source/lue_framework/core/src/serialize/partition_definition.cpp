#include "lue/framework/core/serialize/partition_definition.hpp"
#include <hpx/runtime/serialization/array.hpp>


namespace hpx {
namespace serialization {

// template<
//     typename Index,
//     std::size_t rank>
// void serialize(
//     input_archive& archive,
//     lue::IndexRanges<rank>& ranges,
//     unsigned int const /* version */)
// {
//     for(size_t d = 0; d < rank; ++d) {
//         archive & ranges[d];
//     }
// }
// 
// 
// template<
//     typename Index,
//     std::size_t rank>
// void serialize(
//     input_archive& archive,
//     lue::IndexRanges<rank> const& ranges,
//     unsigned int const /* version */)
// {
//     for(size_t d = 0; d < rank; ++d) {
//         archive & ranges[d];
//     }
// }


template<
    typename Index,
    std::size_t rank>
void serialize(
    input_archive& archive,
    lue::PartitionDefinition<Index, rank>& definition,
    unsigned int const /* version */)
{
    using Definition = lue::PartitionDefinition<Index, rank>;

    typename Definition::Start start;
    typename Definition::Shape shape;

    archive & start & shape;

    definition = Definition{start, shape};
}


template<
    typename Index,
    std::size_t rank>
void serialize(
    output_archive& archive,
    lue::PartitionDefinition<Index, rank> const& definition,
    unsigned int const /* version */)
{
    archive & definition.start() & definition.shape();
}


// template void serialize<1>(
//     input_archive&, lue::PartitionDefinition<1>&, unsigned int const);

template void serialize<std::uint64_t, 2>(
    input_archive&, lue::PartitionDefinition<std::uint64_t, 2>&, unsigned int const);

// template void serialize<3>(
//     input_archive&, lue::PartitionDefinition<3>&, unsigned int const);

// template void serialize<1>(
//     output_archive&, lue::PartitionDefinition<1> const&, unsigned int const);
template void serialize<std::uint64_t, 2>(
    output_archive&, lue::PartitionDefinition<std::uint64_t, 2> const&, unsigned int const);
// template void serialize<3>(
//     output_archive&, lue::PartitionDefinition<3> const&, unsigned int const);

}  // namespace serialization
}  // namespace hpx
