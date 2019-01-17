#pragma once
#include "lue/framework/core/array_partition_definition.hpp"
#include <hpx/runtime/serialization/serialize.hpp>


namespace hpx {
namespace serialization {

template<
    typename Index,
    std::size_t rank>
void               serialize           (
                                    input_archive& archive,
                                    lue::ArrayPartitionDefinition<Index, rank>&
                                        definition,
                                    unsigned int const version);

template<
    typename Index,
    std::size_t rank>
void               serialize           (
                                    output_archive& archive,
                                    lue::ArrayPartitionDefinition<Index, rank>
                                        const& definition,
                                    unsigned int const version);

}  // namespace serialization
}  // namespace hpx
