#pragma once
#include "lue/framework/core/array_partition_visitor.hpp"
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

// template<
//     typename Weight,
//     typename Element>
// Element convolve(
//     Weight const left_weight,
//     Element const left_element,
//     Weight const middle_weight,
//     Element const middle_element,
//     Weight const right_weight,
//     Element const right_element)
// {
//     // 1D, no normalization
//     return
//         left_weight * left_element +
//         middle_weight * middle_element +
//         right_weight * right_element;
// }


// template<
//     typename Array,
//     typename Kernel>
// Array convolve(
//     Array const& array,
//     Kernel const& kernel)
// {
// 
//     // Given the indices of neighboring cells to consider, convolve each
//     // focus cell
// 
//     // 1. We need indices of focus cell along each dimension
//     // 2. We need shape of array
//     // Given 1. and 2. we can update the indices of neighboring cells
//     // to consider. In case neighboring cells are positioned outside of
//     // the array, they need to be handled (skipped, replaced, ...)
// 
// }


// Strategy
// 1. First look at partitions:
//     - In the calculation of each partition there are neighboring
//         partitions that are needed too. Which ones?
//     - Take the size of the neighborhood into consideration. The radius
//         may be larger than the size of each partition. Or just assert
//         that this is not the case.
//     - Also, take the rank into consideration. Along all dimensions
//         there are neighboring partitions.
// 2. Given a partition and its neighboring partitions:
//     - Iterate over each element and convolve it
//     - Request elements from neighboring partitions if needed



// Given an array (of partitions or other kinds of cells), be able to visit
// all elements and have information about the location (indices) within
// the array. It must be possible to pass along a visitor that does
// something at each location.
//
// - ConvolveVisitor: public ConstPartitionVisitor
//     - Visits a partitioned array to prepare for a convolution
//     - Given indices along all dimensions of current partition, grab
//         the neighboring partitions and spawn ConvolvePartitionVisitor
//         visit
//     - Returns a new array with the result of the convolution of the
//         whole array
// - ConvolveArrayVisitor: public ArrayVisitor
//     - Visits a array partition to perform the actual convolution
//     - Returns a new partition with the result of the convolution of
//         the array partition


template<
    typename Array>
class ConvolveVisitor:
    public ConstPartitionVisitor<PartitionsT<Array>>
{

    // - Determine type of output partitions and create collection
    // - During the visit, output partitions can be moved into the
    //   collection

private:

    using Partitions = PartitionsT<Array>;
    using Base = ConstPartitionVisitor<Partitions>;

public:

    ConvolveVisitor(
        Array const& array):

        Base{array.partitions()}

    {
    }

    void operator()()
    {
        // PartitionT<Partitions>& partition = this->partition();

        // ShapeT<Partitions> shrinked_partition_shape{partition.shape().get()};

        // auto const rank = Partitions::rank;

        // for(std::size_t i = 0; i < rank; ++i) {
        //     shrinked_partition_shape[i] =
        //         std::min(shrinked_partition_shape[i], _new_shape[i]);
        // }

        // // TODO Blocks current thread. Maybe wait in destructor?
        // partition.resize(shrinked_partition_shape).wait();
    }

    Array&& result() &&
    {
        return std::move(_result);
    }

private:

    Array          _result;

};


template<
    typename Array,
    typename Kernel>
Array convolve(
    Array const& array,
    Kernel const& kernel)
{
    // TODO
    // - Add ResultElement template argument for type of result elements

    // For each nD focus cell, determine indices of neighboring cells
    // that are positioned within the array

    ConvolveVisitor visitor{array};

    // TODO Add overload visiting the whole array
    // visit_array(visitor);

    return visitor.result();
}

}  // namespace detail

/*
    Image Processing using 2D Convolution:

    The size of the kernel, the numbers within it, and a single normalizer
    value define the operation that is applied to the image.

    The kernel is applied to the image by placing the kernel over the
    image to be convolved and sliding it around to center it over every
    pixel in the original image. At each placement the numbers (pixel
    values) from the original image are multiplied by the kernel number
    that is currently aligned above it.

    The sum of all these products is tabulated and divided by the kernel's
    normalizer. This result is placed into the new image at the position
    of the kernel's center. The kernel is translated to the next pixel
    position and the process repeats until all image pixels have been
    processed.

    Kernel  {#fern_algorithm_convolution_kernel}
    ------
    In image processing, a kernel is a small matrix that contains weights
    to be used to calculate new values based on a source image. For each
    pixel, the kernel is positioned with the center of the kernel on the
    current pixel. Surrounding pixels are weighted by the weights in the
    corresponding kernel cells, and the result is used as the value for
    the current pixel.

    Synonyms: convolution kernel, convolution filer, convolution matrix, mask

    Examples:

    A 3x3 kernel that copies the input to the output:

        0 0 0
        0 1 0
        0 0 0

    A 3x3 kernel that blurs the input image:

        1 1 1
        1 1 1
        1 1 1

    Sharpen:

         0 -1  0
        -1  5 -1
         0 -1  0


    Neighborhood  {#fern_algorithm_convolution_neighborhood}
    ------------
    The neighborhood represents the shape of the non-zero values in the
    kernel. There are different neighborhood shapes:

    - [Moore neighborhood](https://en.wikipedia.org/wiki/Moore_neighborhood)
    - [(Extended) Von Neumann neighborhood](https://en.wikipedia.org/wiki/Von_Neumann_neighborhood)
    - Square.
    - Circle.
*/


/*!
    @brief      Return the result of convolving a kernel with weights
                with a partitioned array
    @tparam     Array Type of partitioned array
    @tparam     Kernel Type of kernel
    @param      array Partitioned array with values to convolve
    @param      kernel Kernel with weights to convolve with
    @return     New partitioned array
*/
template<
    typename Array,
    typename Kernel>
Array convolve(
    Array const& array,
    Kernel const& kernel)
{

    // Convolve algorithm
    // - Split tasks based on whether cells are located at the borders /
    //   corners or not
    // - Split tasks by partition
    // - Handle case where neighborhood radius is larger than partition size
    // - Neighborhood size and contents must be configurable at runtime
    // - Access cells in neighboring partitions


    // Iterate over all partitions
    //     Perform each convolution per partition
    //     Each partition has neighbors
    //         How to keep track of this information
    //     When processing a partition, it must be possible to receive
    //     values from the relevant partitions
    //         Channels seem useful because they support generations


    // A channels can be named

    // How to keep track of channels when the same algorithm may be
    // called multiple time?


    // Upon creation, each component can have a channel associated
    // with it(?), which others can use to obtain elements

    // - A channel has a communication type. Each kind of channel
    //   communicates values of a certain type. In case of convolution,
    //   this is a 'slice/subset' of the neighboring partition: 1D array
    //   partition data in case of 1D partitioned array, 2D array
    //   partition data in case of 2D partitioned array, etc

    // What kind of value a communicator transports depends on the
    // algorithm. This suggests having communicators per algorithm.

    // Are channels useful, or should we just exchange whole
    // partitions? Or both?

    // Creating and destroying lots of communicators seems wasteful. Maybe
    // partitioned arrays should provide (optional?) support for
    // (different kinds of?) communication, which algorithms can use.

    // Communicating subsets of nD arrays on the same locality should not
    // copy values, but only reference them (sharing the array and
    // configuring an nD view?).

    // While iterating over partitions an algorithm implementing a
    // focal operation can pass the partition and the relevant
    // communicators to the convolve_partition action

    // In the convolve_partition action
    // - communicator.set(neighbor, value, generation)
    // - communicator.get(neighbor, generation)

    // How to get generation??? Is it relevant in this scheme?
    // If generations are difficult, channels are less useful to us
    // here. In that case we just need global IDs of the neighboring
    // partitions and have an API to get at values. This limits the
    // asynchronous nature of things, though.

    // stencil_7(285):
    // - A function receiving all partitions relevant for completely
    //   calculating the inner partition
    //   - The border partitions only contain the cells needed for
    //     the calculations


    // Plan: similar to stencil_7 and see where it goes. Generalize to
    //     nD arrays.

    // Order neighbors according to ordering in memory. Try to refrain
    // from using left/right, or north/south, or ...
    // 0: columns
    // 1: rows
    // 2: planes
    // ...

    // entry point is do_work
    // line 356: iterate over all partitions and pass 'focus' partition
    //     and its neighboring partitions to the action that is performed
    //     on the locality of the focus partition (heat_part_action)
    // line 259: calculate a result for a partition. All required
    //     (neighboring) partitions are passed in. This logic executes
    //     on the partition of the focus partition.
    //     - Once focus partition data is received
    //     - Fill inner part of new partition data
    //     - Once that is done and the neighboring partition data is received
    //     - Handle partition borders
    //     - Return new partition
    //     A single function is used that performs the calculation,
    //     given the raw cell values
    //         T = heat(T left, T middle, T right)
    //     → This must be generalized to nD situation and kernels


    // using OutputArray = Array;
    // using OutputPartitions = PartitionsT<OutputArray>;

    // OutputPartitions output_partitions{shape_in_partitions(array)};

    // auto output_partitions = detail::convolve<rank(array)>(array, kernel);

    // return OutputArray{shape(array), std::move(output_partitions)};


    return detail::convolve(array, kernel);




    // // For each partition in the array, determine the locality it is
    // // located on
    // auto localities = locality(array);

    // for(std::size_t p = 0; p < nr_partitions(array); ++p) {

    //     // Call partition convolve action (asynchronously) that convolves
    //     // the current partition, given the surrounding partitions

    //     // The number of surrounding partitions depends on the location
    //     // within the array and the radius of the kernel


    //     // Iterate over partitions
    //     // Select neighboring partitions
    //     // How to do this depends on the rank of the array, doesn't it?
    //     // Can we generalize, or should we just go for 1D and 2D here?


    //     // InputPartition const& input_partition1 = array1.partitions()[p];
    //     // InputPartition const& input_partition2 = array2.partitions()[p];

    //     // output_partitions[p] =
    //     //     hpx::get_colocation_id(input_partition1.get_id()).then(
    //     //         hpx::util::unwrapping(
    //     //             [=](
    //     //                 hpx::id_type const locality_id)
    //     //             {
    //     //                 return hpx::dataflow(
    //     //                     hpx::launch::async,
    //     //                     action,
    //     //                     locality_id,
    //     //                     input_partition1,
    //     //                     input_partition2);
    //     //             }
    //     //         )
    //     //     );

    // }

    // return OutputArray{shape(array), std::move(output_partitions)};
}

}  // namespace lue
