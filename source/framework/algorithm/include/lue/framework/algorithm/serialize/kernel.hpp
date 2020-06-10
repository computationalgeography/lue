#pragma once
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/core/serialize/array.hpp"


namespace hpx {
namespace serialization {

template<
    typename Weight,
    std::size_t rank>
void serialize(
    input_archive& archive,
    lue::Kernel<Weight, rank>& kernel,
    unsigned int const version)
{
    using Array = lue::Array<Weight, rank>;
    using Kernel = lue::Kernel<Weight, rank>;

    // Read array
    Array array;
    serialize(archive, array, version);

    // Assign new instance to kernel passed in
    kernel = Kernel{array};
}


template<
    typename Weight,
    std::size_t rank>
void serialize(
    output_archive& archive,
    lue::Kernel<Weight, rank> const& kernel,
    unsigned int const version)
{
    using Array = lue::Array<Weight, rank>;

    serialize(archive, dynamic_cast<Array const&>(kernel), version);
}

}  // namespace serialization
}  // namespace hpx
