#pragma once
// #include "lue/framework/core/array.hpp"
// #include "lue/framework/core/array_partition_visitor.hpp"
// #include "lue/framework/core/type_traits.hpp"
// #include <hpx/include/lcos.hpp>
// #include <numeric>
// #include <unistd.h>


namespace lue {
namespace detail {





}  // namespace detail



// focal_max: given
// - a partitioned array
//     - Element type is not relevant. As long as the values support
//       being compared using operator<.
// - a kernel
//     - Element type is not relevant. As long as the values support
//       being evaluated as boolean.
// iterate kernel over array and store max value found in kernel window
// in focal cell

// Implementation calls a more generic algorithm accepting a partitioned
// array, a kernel and a functor: focal operation
// This algorithm handles the halo partitions around the array, the
// partitioning of the algorithm, and calling the functor, passing in
// the kernel and the (view on the) elements

// template<
//     typename Array,
//     typename Kernel,
//     typename OutputElement=double>
// PartitionedArrayT<Array, OutputElement> convolve(
//     Array const& array,
//     Kernel const& kernel)
// {
//     detail::Convolution<Array, Kernel, OutputElement> convolution{
//         array, kernel};
// 
//     return convolution();
// 
//     // convolution.start();
// 
//     // return std::move(convolution).result();
// }





}  // namespace lue
