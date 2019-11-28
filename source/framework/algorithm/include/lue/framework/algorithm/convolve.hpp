#pragma once
#include "lue/framework/core/array.hpp"
#include "lue/framework/core/array_partition_visitor.hpp"
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>
#include <numeric>
#include <unistd.h>


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


// template<
//     typename Array>
// class ConvolveVisitor:
//     public ConstPartitionVisitor<PartitionsT<Array>>
// {
// 
//     // - Determine type of output partitions and create collection
//     // - During the visit, output partitions can be moved into the
//     //   collection
// 
// private:
// 
//     using Partitions = PartitionsT<Array>;
//     using Base = ConstPartitionVisitor<Partitions>;
// 
// public:
// 
//     ConvolveVisitor(
//         Array const& array):
// 
//         Base{array.partitions()}
// 
//     {
//     }
// 
//     void operator()()
//     {
//         // PartitionT<Partitions>& partition = this->partition();
// 
//         // ShapeT<Partitions> shrinked_partition_shape{partition.shape().get()};
// 
//         // auto const rank = Partitions::rank;
// 
//         // for(std::size_t i = 0; i < rank; ++i) {
//         //     shrinked_partition_shape[i] =
//         //         std::min(shrinked_partition_shape[i], _new_shape[i]);
//         // }
// 
//         // // TODO Blocks current thread. Maybe wait in destructor?
//         // partition.resize(shrinked_partition_shape).wait();
//     }
// 
//     Array&& result() &&
//     {
//         // Move _result, but only when the type of this instance is an
//         // r-value reference
//         return std::move(_result);
//     }
// 
// private:
// 
//     Array          _result;
// 
// };
// 
// 
// template<
//     typename Array,
//     typename Kernel>
// Array convolve(
//     Array const& array,
//     Kernel const& kernel)
// {
//     // TODO
//     // - Add ResultElement template argument for type of result elements
// 
//     // For each nD focus cell, determine indices of neighboring cells
//     // that are positioned within the array
// 
//     ConvolveVisitor visitor{array};
// 
//     // TODO Add overload visiting the whole array
//     // visit_array(visitor);
// 
//     // return visitor.result();
//     return std::move(visitor).result();
// }


// template<
//     typename Array,
//     typename Kernel>
// Array convolve(
//     Array const& array,
//     Kernel const& kernel)
// {
// }


// template<
//     typename InputPartition,
//     typename OutputPartition,
//     typename Kernel>
// using ConvolvePartitionAction1 =
//     typename convolve::OverloadPicker1<
//         InputPartition, OutputPartition, Kernel>::Action;







// template<
//     typename InputElement,
//     typename OutputElement,
//     typename Kernel>
// OutputElement convolve1(
//     InputElement const* elements,
//     Kernel const& kernel)
// {
//     static_assert(std::is_same_v<ElementT<Kernel>, bool>);
//     static_assert(std::is_convertible_v<InputElement, OutputElement>);
// 
//     OutputElement result = 0;
// 
//     {
//         Count nr_elements = lue::nr_elements(kernel);
//         InputElement const* element = elements - kernel.radius();
//         ElementT<Kernel> const* weight = kernel.data();
// 
//         for(Index i = 0; i < nr_elements; ++i, ++element, ++weight) {
//             if(weight) {
//                 result += *element;
//             }
//         }
//     }
// 
//     return result;
// }
// 
// 
// template<
//     typename InputElement,
//     typename OutputElement,
//     typename Kernel>
// OutputElement convolve1_left_border(
//     InputElement const* left_elements,
//     Count const nr_external_elements,
//     InputElement const* elements,
//     Kernel const& kernel)
// {
//     static_assert(std::is_same_v<ElementT<Kernel>, bool>);
//     static_assert(std::is_convertible_v<InputElement, OutputElement>);
// 
//     OutputElement result = 0;
// 
//     {
//         InputElement const* element = left_elements;
//         ElementT<Kernel> const* weight = kernel.data();
//         Index i;
// 
//         for(i = 0; i < nr_external_elements; ++i, ++element, ++weight) {
//             if(weight) {
//                 result += *element;
//             }
//         }
// 
//         element = elements - kernel.radius() + i + 1;
// 
//         Count nr_elements = lue::nr_elements(kernel);
// 
//         for(; i < nr_elements; ++i, ++element, ++weight) {
//             if(weight) {
//                 result += *element;
//             }
//         }
//     }
// 
//     return result;
// }
// 
// 
// template<
//     typename InputElement,
//     typename OutputElement,
//     typename Kernel>
// OutputElement convolve1_right_border(
//     InputElement const* elements,
//     InputElement const* right_elements,
//     Count const nr_external_elements,
//     Kernel const& kernel)
// {
//     static_assert(std::is_same_v<ElementT<Kernel>, bool>);
//     static_assert(std::is_convertible_v<InputElement, OutputElement>);
// 
//     OutputElement result = 0;
// 
//     {
//         InputElement const* element = elements - kernel.radius();
//         ElementT<Kernel> const* weight = kernel.data();
//         Index i;
// 
//         Count nr_elements = lue::nr_elements(kernel);
// 
//         for(i = 0; i < nr_elements - nr_external_elements;
//                 ++i, ++element, ++weight) {
//             if(weight) {
//                 result += *element;
//             }
//         }
// 
//         element = right_elements;
// 
//         for(; i < nr_elements; ++i, ++element, ++weight) {
//             if(weight) {
//                 result += *element;
//             }
//         }
//     }
// 
//     return result;
// }


/*!
    @brief      Convolve a window of elements
*/
template<
    typename InputElement,
    typename OutputElement,
    typename Subspan,
    typename Kernel>
OutputElement convolve_subspan(
    Subspan const& window,
    Kernel const& kernel)
{
    static_assert(std::is_same_v<ElementT<Kernel>, bool>);
    static_assert(std::is_convertible_v<InputElement, OutputElement>);

    OutputElement result = 0;

    if constexpr(rank<Kernel> == 1) {
        assert(window.extent() == kernel.size());

        for(Index c = 0; c < window.extent(); ++c) {
            if(kernel(c)) {
                result += window(c);
            }
        }
    }
    else if constexpr(rank<Kernel> == 2) {
        assert(window.extent(0) == kernel.size());
        assert(window.extent(1) == kernel.size());

        for(Index r = 0; r < window.extent(0); ++r) {
            for(Index c = 0; c < window.extent(1); ++c) {
                if(kernel(r, c)) {
                    result += window(r, c);
                }
            }
        }
    }

    return result;
}


template<
    typename InputElement,
    typename OutputElement,
    typename Subspans,
    typename Kernel>
OutputElement convolve_border(
    Subspans const& windows,
    Kernel const& kernel)
{
    static_assert(std::is_same_v<ElementT<Kernel>, bool>);
    static_assert(std::is_convertible_v<InputElement, OutputElement>);
    static_assert(rank<Kernel> == rank<Subspans>);

    OutputElement result = 0;

    if constexpr(rank<Kernel> == 1) {
        assert(false);
    }
    else if constexpr(rank<Kernel> == 2) {

#ifndef NDEBUG

        // Although the cells are scattered over multiple partitions,
        // we must see the same number of cells as present in the kernel

        // Check number of columns in each row of partitions
        for(Count r = 0; r < windows.shape()[0]; ++r) {
            Count count = 0;

            for(Count c = 0; c < windows.shape()[1]; ++c) {
                assert(windows(r, c).extent(1) > 0);
                count += windows(r, c).extent(1);
            }

            assert(count == kernel.size());
        }

        // Check number of rows in each column of partitions
        for(Count c = 0; c < windows.shape()[1]; ++c) {
            Count count = 0;

            for(Count r = 0; r < windows.shape()[0]; ++r) {
                assert(windows(r, c).extent(0) > 0);
                count += windows(r, c).extent(0);
            }

            assert(count == kernel.size());
        }

        // Within a row of partitions, the number of rows per
        // partition must be the same
        for(Count r = 0; r < windows.shape()[0]; ++r) {
            Count const nr_rows = windows(r, 0).extent(0);

            for(Count c = 0; c < windows.shape()[1]; ++c) {

                assert(windows(r, c).extent(0) == nr_rows);
            }
        }

        // Within a column of partitions, the number of columns per
        // partition must be the same
        for(Count c = 0; c < windows.shape()[1]; ++c) {
            Count const nr_cols = windows(0, c).extent(1);

            for(Count r = 0; r < windows.shape()[0]; ++r) {
                assert(windows(r, c).extent(1) == nr_cols);
            }
        }

#endif

        // {r,c}w : {row,col} index window

        Index rk = 0;

        // Iterate over all views into partitions
        for(Count rw = 0; rw < windows.shape()[0]; ++rw) {

            Index ck = 0;

            for(Count cw = 0; cw < windows.shape()[1]; ++cw) {

                auto const& window = windows(rw, cw);

                assert(window.extent(0) <= kernel.size());
                assert(window.extent(1) <= kernel.size());

                for(Index r = 0; r < window.extent(0); ++r, ++rk) {
                    for(Index c = 0; c < window.extent(1); ++c, ++ck) {

                        assert(rk < std::get<0>(kernel.shape()));
                        assert(ck < std::get<1>(kernel.shape()));

                        if(kernel(rk, ck)) {
                            result += window(r, c);
                        }

                    }

                    ck -= window.extent(1);  // Carriage return
                }

                // Entering next partition in row

                // Reset kernel row index to start row
                rk -= window.extent(0);

                // Offset kernel col indices by previous partition's cols
                ck += window.extent(1);

            }

            // Offset kernel row indices by previous partition's rows
            rk += windows(rw, 0).extent(0);
        }
    }

    return result;
}


// template<
//     typename InputPartition,
//     typename OutputPartition,
//     typename Kernel>
// OutputPartition convolve_partition1(
//     InputPartition const& left_partition,
//     InputPartition const& center_partition,
//     InputPartition const& right_partition,
//     Kernel const& kernel)
// {
//     assert(
//         hpx::get_colocation_id(center_partition.get_id()).get() ==
//         hpx::find_here());
// 
//     using InputData = DataT<InputPartition>;
//     using InputElement = ElementT<InputPartition>;
// 
//     using OutputData = DataT<OutputPartition>;
//     using OutputElement = ElementT<OutputPartition>;
// 
//     auto convolve = convolve1<InputElement, OutputElement, Kernel>;
//     auto convolve_left_border =
//         convolve1_left_border<InputElement, OutputElement, Kernel>;
//     auto convolve_right_border =
//         convolve1_right_border<InputElement, OutputElement, Kernel>;
// 
//     // When the focal cell is located on the first or last element of
//     // the new partition, radius number of cells are needed from the
//     // neighbording partitions. Asynchronously request them.
// 
//     // Meanwhile, perform calculations for inner section of the new
//     // partition
//     hpx::shared_future<InputData> center_data =
//         center_partition.data(CopyMode::share);
// 
//     auto output_data = hpx::dataflow(
//         hpx::launch::async,
//         hpx::util::unwrapping(
// 
//             [convolve, kernel](
//                 InputData const& center_data)
//             {
//                 OutputData output_data{center_data.shape()};
// 
//                 assert(kernel.radius() == 1);
// 
//                 for(Index i = kernel.radius();
//                         i < center_data.nr_elements() - kernel.radius(); ++i) {
//                     output_data[i] = convolve(&center_data[i], kernel);
//                 }
// 
//                 return output_data;
//             }
// 
//         ),
//         center_data);
// 
//     // Once the elements from the neighboring partitions have arrived,
//     // finish by performing calculations for the sides of the new
//     // partition
//     return hpx::dataflow(
//         hpx::launch::async,
//         hpx::util::unwrapping(
// 
//             [convolve_left_border, convolve_right_border, kernel](
//                 hpx::id_type const locality_id,
//                 OutputData&& output_data,
//                 InputData const& left_data,
//                 InputData const& center_data,
//                 InputData const& right_data)
//             {
//                 assert(kernel.radius() == 1);
//                 assert(nr_elements(left_data) >= kernel.radius());
//                 assert(nr_elements(right_data) >= kernel.radius());
// 
//                 Count const nr_external_elements = kernel.radius();
// 
//                 // Left border
//                 {
//                     Index l = nr_elements(left_data) - kernel.radius();
// 
//                     for(Index i = 0; i < kernel.radius(); ++i, ++l) {
//                         output_data[i] = convolve_left_border(
//                             &left_data[l], nr_external_elements,
//                             &center_data[i], kernel);
//                     }
//                 }
// 
//                 // Right border
//                 {
//                     for(Index i = nr_elements(center_data) -
//                             kernel.radius(), nr_external_elements = 1;
//                             i < nr_elements(center_data);
//                             ++i, ++nr_external_elements) {
// 
//                         output_data[i] = convolve_right_border(
//                             &center_data[i], &right_data[0],
//                             nr_external_elements, kernel);
// 
//                     }
//                 }
// 
//                 return OutputPartition{locality_id, std::move(output_data)};
//             }
// 
//         ),
//         hpx::get_colocation_id(center_partition.get_id()),
//         std::move(output_data),
//         left_partition.data(CopyMode::share),  // FIXME only subset of data
//         center_data,
//         right_partition.data(CopyMode::share));  // FIXME only subset of data
// }


template<
    lue::Rank rank,
    lue::Index dimension=rank>
constexpr lue::Count nr_neighbors()
{
    // Given the rank of a partition, how many neighboring partitions
    // are there?

    // 1D: 1 * 2 * (1 - 0) → 2
    // 2D: 1 * 2 * (2 - 0) + 2 * 2 * (2 - 1) → 4 + 4 → 8
    // 3D: 1 * 2 * (3 - 0) + 2 * 2 * (3 - 1) + 3 * 2 * (3 - 2) → 6 + 8 + 12 → 26
    //
    // Given rank r, for each current dimension d є (0, r]:
    //     d * 2 * (r - (d - 1))

    if constexpr(dimension > 0) {
        return
            dimension * 2 * (rank - (dimension - 1)) +
            nr_neighbors<rank, dimension - 1>();
    }
    else {
        return 0;
    }
}


template<
    typename Partition>
constexpr lue::Count nr_neighbors()
{
    // Given a partition, how many neighboring partitions are there?
    return nr_neighbors<rank<Partition>>();
}


template<
    typename InputPartitions,
    typename OutputPartition,
    typename Kernel>
OutputPartition convolve_partition(
    InputPartitions const& partitions,
    Kernel const& kernel)
{
    using InputPartition = PartitionT<InputPartitions>;
    using InputData = DataT<InputPartition>;
    using InputElement = ElementT<InputPartition>;
    using InputDataSpan = typename InputData::Span;
    using InputDataSubspan = typename InputData::Subspan;

    using OutputData = DataT<OutputPartition>;
    using OutputElement = ElementT<OutputPartition>;

    using Shape = ShapeT<InputPartitions>;

    using Slice = SliceT<InputPartition>;
    using Slices = SlicesT<InputPartition>;

    auto const kernel_radius = kernel.radius();

    assert(partitions.nr_elements() == nr_neighbors<InputPartition>() + 1);

    if constexpr(rank<InputPartition> == 2) {
        assert(
            hpx::get_colocation_id(partitions(1, 1).get_id()).get() ==
            hpx::find_here());

        auto const [nr_partitions0, nr_partitions1] = partitions.shape();

        // The partitions collection contains 9 partitions:
        //
        // +------+-----+------+
        // | 0, 0 | 0, 1| 0, 2 |
        // +------+-----+------+
        // | 1, 0 | 1, 1| 1, 2 |
        // +------+-----+------+
        // | 2, 0 | 2, 1| 2, 2 |
        // +------+-----+------+
        //
        // We only need to calculate a result for partition (1, 1). The
        // other ones are used only to provide input values for the sides
        // of this partition.

        // Create an array with futures to the shape of the array
        // partitions. These shapes are used to obtain the smallest
        // amount of elements from the border partitions necessary to
        // calculate the result for the partition at (1, 1).

        using PartitionShapes =
            Array<hpx::shared_future<Shape>, rank<InputPartition>>;

        PartitionShapes partition_shapes{partitions.shape()};

        for(Index r = 0; r < nr_partitions0; ++r) {
            for(Index c = 0; c < nr_partitions1; ++c) {
                partition_shapes(r, c) = partitions(r, c).shape();
            }
        }

        // Once the shapes have arrived, request the elements for each
        // partition. Which elements to request depends on the location of
        // the partition relative to the current/center partition located at
        // (1, 1).

        // Create an array with futures to partition data instances. Once
        // the data has arrived, these instances are used in the actual
        // calculations.

        using InputPartitionsData =
            Array<hpx::shared_future<InputData>, rank<InputPartition>>;

        InputPartitionsData input_partitions_data{partitions.shape()};

        // North-west corner partition: get south-east corner elements
        input_partitions_data(0, 0) = partition_shapes(0, 0).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(0, 0)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);

                    return partition.slice(
                        Slices{{
                            Slice{nr_elements0 - kernel_radius, nr_elements0},
                            Slice{nr_elements1 - kernel_radius, nr_elements1}
                        }});
                }

            ));

        // North partition: get south side elements
        input_partitions_data(0, 1) = partition_shapes(0, 1).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(0, 1)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);

                    return partition.slice(
                        Slices{{
                            Slice{nr_elements0 - kernel_radius, nr_elements0},
                            Slice{0, nr_elements1}
                        }});
                }

            ));

        // North-east partition: get south-west corner elements
        input_partitions_data(0, 2) = partition_shapes(0, 2).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(0, 2)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);

                    return partition.slice(
                        Slices{{
                            Slice{nr_elements0 - kernel_radius, nr_elements0},
                            Slice{0, kernel_radius}
                        }});
                }

            ));

        // West partition: get east side elements
        input_partitions_data(1, 0) = partition_shapes(1, 0).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(1, 0)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);

                    return partition.slice(
                        Slices{{
                            Slice{0, nr_elements0},
                            Slice{nr_elements1 - kernel_radius, nr_elements1}
                        }});
                }

            ));

        // Center partition: get all elements
        input_partitions_data(1, 1) = partitions(1, 1).data(CopyMode::share);

        // East partition: get west side elements
        input_partitions_data(1, 2) = partition_shapes(1, 2).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(1, 2)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);

                    return partition.slice(
                        Slices{{
                            Slice{0, nr_elements0},
                            Slice{0, kernel_radius}
                        }});
                }

            ));

        // South-west partition: get north-east corner elements
        input_partitions_data(2, 0) = partition_shapes(2, 0).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(2, 0)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);

                    return partition.slice(
                        Slices{{
                            Slice{0, kernel_radius},
                            Slice{nr_elements1 - kernel_radius, nr_elements1}
                        }});
                }

            ));

        // South partition: get north side elements
        input_partitions_data(2, 1) = partition_shapes(2, 1).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(2, 1)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);

                    return partition.slice(
                        Slices{{
                            Slice{0, kernel_radius},
                            Slice{0, nr_elements1}
                        }});
                }

            ));

        // South-east partition: get north-west corner elements
        input_partitions_data(2, 2) = partition_shapes(2, 2).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(2, 2)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);

                    return partition.slice(
                        Slices{{
                            Slice{0, kernel_radius},
                            Slice{0, kernel_radius}
                        }});
                }

            ));


        hpx::future<hpx::id_type> locality_id_future =
            hpx::get_colocation_id(partitions(1, 1).get_id());


        // Once the elements from the center partition have arrived,
        // perform calculations for all cells whose neighborhood are
        // contained within this partition
        auto output_data_future = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                [kernel](
                    InputData const& partition_data)
                {
                    auto const [nr_rows, nr_cols] = partition_data.shape();
                    auto const& array_span = partition_data.span();

                    assert(nr_rows >= kernel.size());
                    assert(nr_cols >= kernel.size());

                    OutputData output_data{partition_data.shape()};

                    // rf, cf are indices of focal cell in array
                    // ck, ck are indices of first cell in array
                    //     that is visible from kernel

                    for(Index rf = kernel.radius(), rk = rf - kernel.radius();
                            rf < nr_rows - kernel.radius(); ++rf, ++rk) {
                        for(Index cf = kernel.radius(), ck = cf - kernel.radius();
                                cf < nr_cols - kernel.radius(); ++cf, ++rk) {

                            output_data(rf, cf) =
                                convolve_subspan<InputElement, OutputElement>(
                                    lue::subspan(array_span,
                                        Slice{rk, rk + kernel.size()},
                                        Slice{ck, ck + kernel.size()}),
                                    kernel);

                        }
                    }

                    return output_data;
                }

            ),
            input_partitions_data(1, 1));

        // Once the elements from all neighboring partitions have arrived,
        // finish by performing calculations for the sides of the new
        // partition

        auto input_partitions_data_future = hpx::when_all_n(
             input_partitions_data.begin(),
             nr_elements(input_partitions_data.shape()));

        return hpx::when_all(
                input_partitions_data_future,
                locality_id_future,
                output_data_future).then(
            hpx::util::unwrapping(

                [kernel](
                    auto&& data)
                {
                    auto input_partitions_data_futures =
                        hpx::util::get<0>(data).get();
                    hpx::id_type const locality_id =
                        hpx::util::get<1>(data).get();
                    OutputData output_partition_data =
                        hpx::util::get<2>(data).get();

                    using InputPartitionsData =
                        Array<InputData, rank<InputPartition>>;
                    InputPartitionsData partitions_data{Shape{{3, 3}}};

                    std::transform(
                            input_partitions_data_futures.begin(),
                            input_partitions_data_futures.end(),
                            partitions_data.begin(),

                            [](auto&& future) {
                                return future.get();
                            }

                        );

                    using InputPartitionsDataSpan =
                        Array<InputDataSpan, rank<InputPartition>>;
                    InputPartitionsDataSpan partition_spans{Shape{{3, 3}}};

                    std::transform(
                            partitions_data.begin(),
                            partitions_data.end(),
                            partition_spans.begin(),

                            [](InputData const& input_partition_data) {
                                return input_partition_data.span();
                            }

                        );

                    using InputPartitionsDataSubspan =
                        Array<InputDataSubspan, rank<InputPartition>>;

                    auto const [nr_elements0, nr_elements1] =
                        partitions_data(1, 1).shape();

                    // rf, cf are indices of focal cell in array
                    // ck, ck are indices of first cell in array
                    //     that is visible from kernel

                    {
                        // Four partitions are involved when calculating
                        // the convolution for the corner cells. In this
                        // object we store the views into these partitions
                        // which are used to calculate the convolution for
                        // a single focal cell.
                        InputPartitionsDataSubspan windows{Shape{{2, 2}}};

                        // Indices of the four partition views used in the corners
                        // {row, col}, {north-west, ..., south-west}, {begin, end}
                        // → {r,c}{nw,ne,se,sw}{b,e}
                        Index rnwb, rnwe, cnwb, cnwe;
                        Index rneb, rnee, cneb, cnee;
                        Index rswb, rswe, cswb, cswe;
                        Index rseb, rsee, cseb, csee;

                        // North-west corner
                        {
                            auto const& nw_partition = partition_spans(0, 0);
                            auto const& ne_partition = partition_spans(0, 1);
                            auto const& sw_partition = partition_spans(1, 0);
                            auto const& se_partition = partition_spans(1, 1);

                            rnwb = nw_partition.extent(0) - kernel.radius();
                            rnwe = nw_partition.extent(0);
                            cnwe = nw_partition.extent(1);

                            rneb = ne_partition.extent(0) - kernel.radius();
                            rnee = ne_partition.extent(0);
                            cneb = 0;

                            rswb = 0;
                            rswe = kernel.radius() + 1;
                            cswe = sw_partition.extent(1);

                            rseb = 0;
                            rsee = kernel.radius() + 1;
                            cseb = 0;

                            for(Index rf = 0; rf < kernel.radius();
                                    ++rf, ++rnwb, ++rneb, ++rswe, ++rsee) {

                                cnwb = nw_partition.extent(1) - kernel.radius();
                                cnee = kernel.radius() + 1;
                                cswb = sw_partition.extent(1) - kernel.radius();
                                csee = kernel.radius() + 1;

                                for(Index cf = 0; cf < kernel.radius();
                                        ++cf, ++cnwb, ++cnee, ++cswb, ++csee) {

                                    // NW partition view
                                    windows(0, 0) = lue::subspan(nw_partition,
                                        Slice{rnwb, rnwe}, Slice{cnwb, cnwe});
                                    assert(windows(0, 0).extent(0) > 0);
                                    assert(windows(0, 0).extent(1) > 0);
                                    assert(windows(0, 0).extent(0) <= kernel.radius());
                                    assert(windows(0, 0).extent(1) <= kernel.radius());

                                    // NE partition view
                                    windows(0, 1) = lue::subspan(ne_partition,
                                        Slice{rneb, rnee}, Slice{cneb, cnee});
                                    assert(windows(0, 1).extent(0) > 0);
                                    assert(windows(0, 1).extent(1) > 0);
                                    assert(windows(0, 1).extent(0) <= kernel.radius());
                                    assert(windows(0, 1).extent(1) < kernel.size());

                                    // SW partition view
                                    windows(1, 0) = lue::subspan(sw_partition,
                                        Slice{rswb, rswe}, Slice{cswb, cswe});
                                    assert(windows(1, 0).extent(0) > 0);
                                    assert(windows(1, 0).extent(1) > 0);
                                    assert(windows(1, 0).extent(0) < kernel.size());
                                    assert(windows(1, 0).extent(1) <= kernel.radius());

                                    // SE partition view
                                    windows(1, 1) = lue::subspan(se_partition,
                                        Slice{rseb, rsee}, Slice{cseb, csee});
                                    assert(windows(1, 1).extent(0) > 0);
                                    assert(windows(1, 1).extent(1) > 0);
                                    assert(windows(1, 1).extent(0) < kernel.size());
                                    assert(windows(1, 1).extent(1) < kernel.size());

                                    output_partition_data(rf, cf) =
                                        convolve_border<InputElement, OutputElement>(
                                            windows, kernel);
                                }
                            }
                        }

                        // North-east corner
                        {
                            auto const& nw_partition = partition_spans(0, 1);
                            auto const& ne_partition = partition_spans(0, 2);
                            auto const& sw_partition = partition_spans(1, 1);
                            auto const& se_partition = partition_spans(1, 2);

                            rnwb = nw_partition.extent(0) - kernel.radius();
                            rnwe = nw_partition.extent(0);
                            cnwe = nw_partition.extent(1);

                            rneb = ne_partition.extent(0) - kernel.radius();
                            rnee = ne_partition.extent(0);
                            cneb = 0;

                            rswb = 0;
                            rswe = kernel.radius() + 1;
                            cswe = sw_partition.extent(1);

                            rseb = 0;
                            rsee = kernel.radius() + 1;
                            cseb = 0;

                            for(Index rf = 0; rf < kernel.radius();
                                    ++rf, ++rnwb, ++rneb, ++rswe, ++rsee) {

                                cnwb = nw_partition.extent(1) - (kernel.size() - 1);
                                cnee = cneb + 1;
                                csee = cseb + 1;
                                cswb = sw_partition.extent(1) - (kernel.size() - 1);

                                for(Index cf = nr_elements1 - kernel.radius(); cf < nr_elements1;
                                        ++cf, ++cnwb, ++cnee, ++cswb, ++csee) {

                                    // NW partition view
                                    windows(0, 0) = lue::subspan(nw_partition,
                                        Slice{rnwb, rnwe}, Slice{cnwb, cnwe});
                                    assert(windows(0, 0).extent(0) > 0);
                                    assert(windows(0, 0).extent(1) > 0);
                                    assert(windows(0, 0).extent(0) <= kernel.radius());
                                    assert(windows(0, 0).extent(1) < kernel.size());

                                    // NE partition view
                                    windows(0, 1) = lue::subspan(ne_partition,
                                        Slice{rneb, rnee}, Slice{cneb, cnee});
                                    assert(windows(0, 1).extent(0) > 0);
                                    assert(windows(0, 1).extent(1) > 0);
                                    assert(windows(0, 1).extent(0) <= kernel.radius());
                                    assert(windows(0, 1).extent(1) <= kernel.radius());

                                    // SW partition view
                                    windows(1, 0) = lue::subspan(sw_partition,
                                        Slice{rswb, rswe}, Slice{cswb, cswe});
                                    assert(windows(1, 0).extent(0) > 0);
                                    assert(windows(1, 0).extent(1) > 0);
                                    assert(windows(1, 0).extent(0) < kernel.size());
                                    assert(windows(1, 0).extent(1) < kernel.size());

                                    // SE partition view
                                    windows(1, 1) = lue::subspan(se_partition,
                                        Slice{rseb, rsee}, Slice{cseb, csee});
                                    assert(windows(1, 1).extent(0) > 0);
                                    assert(windows(1, 1).extent(1) > 0);
                                    assert(windows(1, 1).extent(0) < kernel.size());
                                    assert(windows(1, 1).extent(1) <= kernel.radius());

                                    output_partition_data(rf, cf) =
                                        convolve_border<InputElement, OutputElement>(
                                            windows, kernel);
                                }
                            }
                        }

                        // South-west corner
                        {
                            auto const& nw_partition = partition_spans(1, 0);
                            auto const& ne_partition = partition_spans(1, 1);
                            auto const& sw_partition = partition_spans(2, 0);
                            auto const& se_partition = partition_spans(2, 1);

                            rnwb = nw_partition.extent(0) - (kernel.size() - 1);
                            rnwe = nw_partition.extent(0);
                            cnwe = nw_partition.extent(1);

                            rneb = ne_partition.extent(0) - (kernel.size() - 1);
                            rnee = ne_partition.extent(0);
                            cneb = 0;

                            rswb = 0;
                            rswe = rswb + 1;
                            cswe = sw_partition.extent(1);

                            rseb = 0;
                            rsee = rseb + 1;
                            cseb = 0;

                            for(Index rf = nr_elements0 - kernel.radius(); rf < nr_elements0;
                                    ++rf, ++rnwb, ++rneb, ++rswe, ++rsee) {

                                cnwb = nw_partition.extent(1) - kernel.radius();
                                cnee = kernel.radius() + 1;
                                cswb = sw_partition.extent(1) - kernel.radius();
                                csee = kernel.radius() + 1;

                                for(Index cf = 0; cf < kernel.radius();
                                        ++cf, ++cnwb, ++cnee, ++cswb, ++csee) {

                                    // NW partition view
                                    windows(0, 0) = lue::subspan(nw_partition,
                                        Slice{rnwb, rnwe}, Slice{cnwb, cnwe});
                                    assert(windows(0, 0).extent(0) > 0);
                                    assert(windows(0, 0).extent(1) > 0);
                                    assert(windows(0, 0).extent(0) < kernel.size());
                                    assert(windows(0, 0).extent(1) <= kernel.radius());

                                    // NE partition view
                                    windows(0, 1) = lue::subspan(ne_partition,
                                        Slice{rneb, rnee}, Slice{cneb, cnee});
                                    assert(windows(0, 1).extent(0) > 0);
                                    assert(windows(0, 1).extent(1) > 0);
                                    assert(windows(0, 1).extent(0) < kernel.size());
                                    assert(windows(0, 1).extent(1) < kernel.size());

                                    // SW partition view
                                    windows(1, 0) = lue::subspan(sw_partition,
                                        Slice{rswb, rswe}, Slice{cswb, cswe});
                                    assert(windows(1, 0).extent(0) > 0);
                                    assert(windows(1, 0).extent(1) > 0);
                                    assert(windows(1, 0).extent(0) <= kernel.radius());
                                    assert(windows(1, 0).extent(1) <= kernel.radius());

                                    // SE partition view
                                    windows(1, 1) = lue::subspan(se_partition,
                                        Slice{rseb, rsee}, Slice{cseb, csee});
                                    assert(windows(1, 1).extent(0) > 0);
                                    assert(windows(1, 1).extent(1) > 0);
                                    assert(windows(1, 1).extent(0) <= kernel.radius());
                                    assert(windows(1, 1).extent(1) < kernel.size());

                                    output_partition_data(rf, cf) =
                                        convolve_border<InputElement, OutputElement>(
                                            windows, kernel);
                                }
                            }
                        }

                        // South-east corner
                        {
                            auto const& nw_partition = partition_spans(1, 1);
                            auto const& ne_partition = partition_spans(1, 2);
                            auto const& sw_partition = partition_spans(2, 1);
                            auto const& se_partition = partition_spans(2, 2);

                            rnwb = nw_partition.extent(0) - (kernel.size() - 1);
                            rnwe = nw_partition.extent(0);
                            cnwe = nw_partition.extent(1);

                            rneb = ne_partition.extent(0) - (kernel.size() - 1);
                            rnee = ne_partition.extent(0);
                            cneb = 0;

                            rswb = 0;
                            rswe = rswb + 1;
                            cswe = sw_partition.extent(1);

                            rseb = 0;
                            rsee = rseb + 1;
                            cseb = 0;

                            for(Index rf = nr_elements0 - kernel.radius(); rf < nr_elements0;
                                    ++rf, ++rnwb, ++rneb, ++rswe, ++rsee) {

                                cnwb = nw_partition.extent(1) - (kernel.size() - 1);
                                cnee = cneb + 1;
                                cswb = sw_partition.extent(1) - (kernel.size() - 1);
                                csee = cseb + 1;

                                for(Index cf = nr_elements1 - kernel.radius(); cf < nr_elements1;
                                        ++cf, ++cnwb, ++cnee, ++cswb, ++csee) {

                                    // NW partition view
                                    windows(0, 0) = lue::subspan(nw_partition,
                                        Slice{rnwb, rnwe}, Slice{cnwb, cnwe});
                                    assert(windows(0, 0).extent(0) > 0);
                                    assert(windows(0, 0).extent(1) > 0);
                                    assert(windows(0, 0).extent(0) < kernel.size());
                                    assert(windows(0, 0).extent(1) < kernel.size());

                                    // NE partition view
                                    windows(0, 1) = lue::subspan(ne_partition,
                                        Slice{rneb, rnee}, Slice{cneb, cnee});
                                    assert(windows(0, 1).extent(0) > 0);
                                    assert(windows(0, 1).extent(1) > 0);
                                    assert(windows(0, 1).extent(0) < kernel.size());
                                    assert(windows(0, 1).extent(1) <= kernel.radius());

                                    // SW partition view
                                    windows(1, 0) = lue::subspan(sw_partition,
                                        Slice{rswb, rswe}, Slice{cswb, cswe});
                                    assert(windows(1, 0).extent(0) > 0);
                                    assert(windows(1, 0).extent(1) > 0);
                                    assert(windows(1, 0).extent(0) <= kernel.radius());
                                    assert(windows(1, 0).extent(1) < kernel.size());

                                    // SE partition view
                                    windows(1, 1) = lue::subspan(se_partition,
                                        Slice{rseb, rsee}, Slice{cseb, csee});
                                    assert(windows(1, 1).extent(0) > 0);
                                    assert(windows(1, 1).extent(1) > 0);
                                    assert(windows(1, 1).extent(0) <= kernel.radius());
                                    assert(windows(1, 1).extent(1) <= kernel.radius());

                                    output_partition_data(rf, cf) =
                                        convolve_border<InputElement, OutputElement>(
                                            windows, kernel);
                                }
                            }
                        }
                    }

                    {
                        // Two partitions are involved when calculating
                        // the convolution for the side cells.

                        // North side
                        {
                            InputPartitionsDataSubspan windows{Shape{{2, 1}}};

                            auto const& n_partition = partition_spans(0, 1);
                            auto const& s_partition = partition_spans(1, 1);

                            Index rnb = n_partition.extent(0) - kernel.radius();
                            Index const rne = n_partition.extent(0);
                            Index const rsb = 0;
                            Index rse = kernel.radius() + 1;

                            for(Index rf = 0; rf < kernel.radius();
                                    ++rf, ++rnb, ++rse) {

                                for(Index cf = kernel.radius(), cb = 0; cf < nr_elements1 - kernel.radius();
                                        ++cf, ++cb) {

                                    // N partition view
                                    windows(0, 0) = lue::subspan(n_partition,
                                        Slice{rnb, rne}, Slice{cb, cb + kernel.size()});
                                    assert(windows(0, 0).extent(0) > 0);
                                    assert(windows(0, 0).extent(1) > 0);
                                    assert(windows(0, 0).extent(0) <= kernel.radius());
                                    assert(windows(0, 0).extent(1) == kernel.size());

                                    // S partition view
                                    windows(1, 0) = lue::subspan(s_partition,
                                        Slice{rsb, rse}, Slice{cb, cb + kernel.size()});
                                    assert(windows(1, 0).extent(0) > 0);
                                    assert(windows(1, 0).extent(1) > 0);
                                    assert(windows(1, 0).extent(0) < kernel.size());
                                    assert(windows(1, 0).extent(1) == kernel.size());

                                    output_partition_data(rf, cf) =
                                        convolve_border<InputElement, OutputElement>(
                                            windows, kernel);

                                }
                            }
                        }

                        // West side
                        {
                            InputPartitionsDataSubspan windows{Shape{{1, 2}}};

                            auto const& w_partition = partition_spans(1, 0);
                            auto const& e_partition = partition_spans(1, 1);

                            Index const cwe = w_partition.extent(1);
                            Index const ceb = 0;

                            for(Index rf = kernel.radius(), rb = 0; rf < nr_elements0 - kernel.radius();
                                    ++rf, ++rb) {

                                Index cwb = w_partition.extent(1) - kernel.radius();
                                Index cee = kernel.radius() + 1;

                                for(Index cf = 0; cf < kernel.radius();
                                        ++cf, ++cwb, ++cee) {

                                    // W partition view
                                    windows(0, 0) = lue::subspan(w_partition,
                                        Slice{rb, rb + kernel.size()}, Slice{cwb, cwe});
                                    assert(windows(0, 0).extent(0) > 0);
                                    assert(windows(0, 0).extent(1) > 0);
                                    assert(windows(0, 0).extent(0) == kernel.size());
                                    assert(windows(0, 0).extent(1) <= kernel.radius());

                                    // E partition view
                                    windows(0, 1) = lue::subspan(e_partition,
                                        Slice{rb, rb + kernel.size()}, Slice{ceb, cee});
                                    assert(windows(0, 1).extent(0) > 0);
                                    assert(windows(0, 1).extent(1) > 0);
                                    assert(windows(0, 1).extent(0) == kernel.size());
                                    assert(windows(0, 1).extent(1) < kernel.size());

                                    output_partition_data(rf, cf) =
                                        convolve_border<InputElement, OutputElement>(
                                            windows, kernel);

                                }
                            }
                        }

                        // East side
                        {
                            InputPartitionsDataSubspan windows{Shape{{1, 2}}};

                            auto const& w_partition = partition_spans(1, 1);
                            auto const& e_partition = partition_spans(1, 2);

                            Index const cwe = w_partition.extent(1);
                            Index const ceb = 0;

                            for(Index rf = kernel.radius(), rb = 0; rf < nr_elements0 - kernel.radius();
                                    ++rf, ++rb) {

                                Index cwb = w_partition.extent(1) - (kernel.size() - 1);
                                Index cee = ceb + 1;

                                for(Index cf = nr_elements1 - kernel.radius(); cf < nr_elements1;
                                        ++cf, ++cwb, ++cee) {

                                    // W partition view
                                    windows(0, 0) = lue::subspan(w_partition,
                                        Slice{rb, rb + kernel.size()}, Slice{cwb, cwe});
                                    assert(windows(0, 0).extent(0) > 0);
                                    assert(windows(0, 0).extent(1) > 0);
                                    assert(windows(0, 0).extent(0) == kernel.size());
                                    assert(windows(0, 0).extent(1) < kernel.size());

                                    // E partition view
                                    windows(0, 1) = lue::subspan(e_partition,
                                        Slice{rb, rb + kernel.size()}, Slice{ceb, cee});
                                    assert(windows(0, 1).extent(0) > 0);
                                    assert(windows(0, 1).extent(1) > 0);
                                    assert(windows(0, 1).extent(0) == kernel.size());
                                    assert(windows(0, 1).extent(1) <= kernel.radius());

                                    output_partition_data(rf, cf) =
                                        convolve_border<InputElement, OutputElement>(
                                            windows, kernel);

                                }
                            }
                        }

                        // South side
                        {
                            InputPartitionsDataSubspan windows{Shape{{2, 1}}};

                            auto const& n_partition = partition_spans(1, 1);
                            auto const& s_partition = partition_spans(2, 1);

                            Index rnb = n_partition.extent(0) - (kernel.size() - 1);
                            Index const rne = n_partition.extent(0);

                            Index const rsb = 0;
                            Index rse = rsb + 1;

                            for(Index rf = nr_elements0 - kernel.radius(); rf < nr_elements0;
                                    ++rf, ++rnb, ++rse) {

                                for(Index cf = kernel.radius(), cb = 0; cf < nr_elements1 - kernel.radius();
                                        ++cf, ++cb) {

                                    // N partition view
                                    windows(0, 0) = lue::subspan(n_partition,
                                        Slice{rnb, rne}, Slice{cb, cb + kernel.size()});
                                    assert(windows(0, 0).extent(0) > 0);
                                    assert(windows(0, 0).extent(1) > 0);
                                    assert(windows(0, 0).extent(0) < kernel.size());
                                    assert(windows(0, 0).extent(1) == kernel.size());

                                    // S partition view
                                    windows(1, 0) = lue::subspan(s_partition,
                                        Slice{rsb, rse}, Slice{cb, cb + kernel.size()});
                                    assert(windows(1, 0).extent(0) > 0);
                                    assert(windows(1, 0).extent(1) > 0);
                                    assert(windows(1, 0).extent(0) <= kernel.radius());
                                    assert(windows(1, 0).extent(1) == kernel.size());

                                    output_partition_data(rf, cf) =
                                        convolve_border<InputElement, OutputElement>(
                                            windows, kernel);

                                }
                            }
                        }
                    }

                    // Done, create and return the output partition ------------
                    return OutputPartition{
                        locality_id, std::move(output_partition_data)};
                }

            ));


        // return hpx::dataflow(
        //     hpx::launch::async,
        //     hpx::util::unwrapping(

        //         // [convolve_left_border, convolve_right_border, kernel](
        //         [kernel](
        //             hpx::id_type const locality_id,
        //             OutputData&& output_data)
        //             // InputData const& left_data,
        //             // InputData const& center_data,
        //             // InputData const& right_data)
        //         {
        //             // assert(kernel.radius() == 1);
        //             // assert(nr_elements(left_data) >= kernel.radius());
        //             // assert(nr_elements(right_data) >= kernel.radius());

        //             // Count const nr_external_elements = kernel.radius();

        //             // // Left border
        //             // {
        //             //     Index l = nr_elements(left_data) - kernel.radius();

        //             //     for(Index i = 0; i < kernel.radius(); ++i, ++l) {
        //             //         output_data[i] = convolve_left_border(
        //             //             &left_data[l], nr_external_elements,
        //             //             &center_data[i], kernel);
        //             //     }
        //             // }

        //             // // Right border
        //             // {
        //             //     for(Index i = nr_elements(center_data) -
        //             //             kernel.radius(), nr_external_elements = 1;
        //             //             i < nr_elements(center_data);
        //             //             ++i, ++nr_external_elements) {

        //             //         output_data[i] = convolve_right_border(
        //             //             &center_data[i], &right_data[0],
        //             //             nr_external_elements, kernel);

        //             //     }
        //             // }

        //             return OutputPartition{locality_id, std::move(output_data)};
        //         }

        //     ),
        //     hpx::get_colocation_id(partitions(1, 1).get_id()),
        //     std::move(output_data));



        //     // left_partition.data(CopyMode::share),  // FIXME only subset of data
        //     // center_data,
        //     // right_partition.data(CopyMode::share));  // FIXME only subset of data
    }

    assert(false);
    return OutputPartition{hpx::find_here()};
}


// template<
//     typename InputPartition,
//     typename OutputPartition,
//     typename Kernel>
// struct ConvolvePartitionAction1:
//     hpx::actions::make_action<
//         decltype(&convolve_partition1<InputPartition, OutputPartition, Kernel>),
//         &convolve_partition1<InputPartition, OutputPartition, Kernel>,
//         ConvolvePartitionAction1<InputPartition, OutputPartition, Kernel>>
// {};


template<
    typename InputPartitions,
    typename OutputPartition,
    typename Kernel>
struct ConvolvePartitionAction:
    hpx::actions::make_action<
        decltype(&convolve_partition<InputPartitions, OutputPartition, Kernel>),
        &convolve_partition<InputPartitions, OutputPartition, Kernel>,
        ConvolvePartitionAction<InputPartitions, OutputPartition, Kernel>>
{};


// template<
//     typename Array,
//     typename Kernel,
//     typename OutputElement>
// PartitionedArrayT<Array, OutputElement> convolve_1d(
//     Array const& array,
//     Kernel const& kernel)
// {
//     static_assert(lue::rank<Array> == 1);
// 
//     using InputArray = Array;
//     using InputPartition = PartitionT<InputArray>;
// 
//     using OutputArray = PartitionedArrayT<Array, OutputElement>;
//     using OutputPartitions = PartitionsT<OutputArray>;
//     using OutputPartition = PartitionT<OutputArray>;
// 
//     ConvolvePartitionAction1<InputPartition, OutputPartition, Kernel> action;
//     OutputPartitions output_partitions{shape_in_partitions(array)};
// 
//     // +---+---+---+---+---+
//     // | 1 | 1 | 1 | 1 | 1 |
//     // +---+---+---+---+---+
// 
//     // *
// 
//     // +------+------+------+
//     // | true | true | true |
//     // +------+------+------+
// 
//     // =
// 
//     // +---+---+---+---+---+
//     // | 2 | 3 | 3 | 3 | 2 |
//     // +---+---+---+---+---+
// 
//     // - We assume that for all output elements in a partition a value
//     //     can be calculated based on the input partition containing
//     //     the focal element and the bordering partitions. This implies
//     //     that the size of each partition must be at least as large as
//     //     the kernel radius.
// 
//     // Inner partitions: all partitions except for the first and last one
//     //     act(location, partition[-1], partition, partition[+1])
// 
//     // Border partitions:
//     //     act(location, partition, partition[+1])
//     //     act(location, partition[-1], partition)
// 
//     auto const nr_partitions = lue::nr_partitions(array);
// 
//     if(nr_partitions == 1) {
//         // Pass in ghost partitions for left and right partition
//         // TODO
//         assert(false);
//     }
//     else if(nr_partitions >= 2) {
// 
//         // Inner partitions
//         for(Index p = 1; p < nr_partitions - 1; ++p) {
// 
//             output_partitions[p] = hpx::dataflow(
//                 hpx::launch::async,
// 
//                 [action, kernel](
//                     InputPartition const& left_partition,
//                     InputPartition const& center_partition,
//                     InputPartition const& right_partition)
//                 {
//                     return action(
//                         hpx::get_colocation_id(
//                             hpx::launch::sync, center_partition.get_id()),
//                         left_partition,
//                         center_partition,
//                         right_partition,
//                         kernel);
//                 },
// 
//                 array.partitions()[p-1],
//                 array.partitions()[p],
//                 array.partitions()[p+1]);
// 
//         }
// 
// 
// 
// 
//         // First partition: pass in ad-hoc partition for left partition
//         // TODO
// 
//         // Last partition: pass in ad-hoc partition for right partition
//         // TODO
// 
//         // Options
//         // - Create halo of real partitions around raster, with just
//         //     enough cells to be able to perform the calculations
//         // - Create partitions that only contain logic, no data, but
//         //     behave as real partitions
//         // - Do all this in the actions that handle borders and corners
//         // - Don't use halo partitions, but special border/corner actions
//         //     and policies for how to handle halo cells
// 
//     }
// 
//     return OutputArray{shape(array), std::move(output_partitions)};
// }


template<
    typename Array,
    typename Kernel,
    typename OutputElement>
PartitionedArrayT<Array, OutputElement> convolve_2d(
    Array const& input_array,
    Kernel const& kernel)
{
    static_assert(lue::rank<Array> == 2);

    using InputArray = Array;
    using InputPartition = PartitionT<InputArray>;
    using InputPartitions = PartitionsT<InputArray>;

    using OutputArray = PartitionedArrayT<Array, OutputElement>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;

    using Shape = ShapeT<Array>;

    ConvolvePartitionAction<InputPartitions, OutputPartition, Kernel> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    // auto const nr_partitions = lue::nr_partitions(input_array);
    auto const [nr_partitions0, nr_partitions1] =
        lue::shape_in_partitions(input_array);
    auto const kernel_radius = kernel.radius();

    // Iterate over all partitions. Per partition determine the collection
    // of neighboring partitions, and asynchronously call the algorithm
    // that performs the calculations.



    // -------------------------------------------------------------------------
    // Create a halo of temporary partitions that are used in the
    // convolution of the partitions along the borders of the
    // array. We create three collections of partitions:
    // - 2x2 partitions for the corners of the halo
    // - 2xc partitions for the longitudinal sides of the halo
    // - rx2 partitions for the latitudinal sides of the halo
    //
    // The size of these partitions is as small as possible. They
    // contain the minimum amount of elements needed for the
    // calculations.
    //
    // The halo partitions are located on the same locality as the
    // nearest border partition in the array.

    // Corner halo partitions
    //     +----+----+
    //     | NW | NE |
    //     +----+----+
    //     | SW | SE |
    //     +----+----+
    InputPartitions halo_corner_partitions{Shape{{2, 2}}};

    // North-west corner halo partition
    halo_corner_partitions(0, 0) = hpx::get_colocation_id(
            input_array.partitions()(0, 0).get_id()).then(
        hpx::util::unwrapping(

            [kernel_radius](
                hpx::id_type const locality_id)
            {
                return InputPartition{locality_id,
                    Shape{{kernel_radius, kernel_radius}}, 0};
            }

        ));

    // North-east corner halo partition
    halo_corner_partitions(0, 1) = hpx::get_colocation_id(
            input_array.partitions()(0, nr_partitions1 - 1).get_id()).then(
        hpx::util::unwrapping(

            [kernel_radius](
                hpx::id_type const locality_id)
            {
                return InputPartition{locality_id,
                    Shape{{kernel_radius, kernel_radius}}, 0};
            }

        ));

    // South-west corner halo partition
    halo_corner_partitions(1, 0) = hpx::get_colocation_id(
            input_array.partitions()(nr_partitions0 - 1, 0).get_id()).then(
        hpx::util::unwrapping(

            [kernel_radius](
                hpx::id_type const locality_id)
            {
                return InputPartition{locality_id,
                    Shape{{kernel_radius, kernel_radius}}, 0};
            }

        ));

    // South-east corner halo partition
    halo_corner_partitions(1, 1) = hpx::get_colocation_id(
            input_array.partitions()(nr_partitions0 - 1, nr_partitions1 - 1).get_id()).then(
        hpx::util::unwrapping(

            [kernel_radius](
                hpx::id_type const locality_id)
            {
                return InputPartition{locality_id,
                    Shape{{kernel_radius, kernel_radius}}, 0};
            }

        ));

    // Longitudinal side halo partitions
    //     +---+---+---+
    //     | N | N | N |
    //     +---+---+---+
    //     | S | S | S |
    //     +---+---+---+
    InputPartitions halo_longitudinal_side_partitions{
        Shape{{2, nr_partitions1}}};

    for(auto const [rh, rp]: {
            std::array<Index, 2>{{0, 0}},
            std::array<Index, 2>{{1, nr_partitions0 - 1}}}) {
        for(Index cp = 0; cp < nr_partitions1; ++cp) {

            halo_longitudinal_side_partitions(rh, cp) = hpx::dataflow(
                hpx::launch::async,
                hpx::util::unwrapping(

                    [kernel_radius](
                        hpx::id_type const locality_id,
                        Shape const& partition_shape)
                    {
                        return InputPartition{locality_id,
                            Shape{{kernel_radius, partition_shape[1]}}, 0};
                    }

                ),

                hpx::get_colocation_id(
                    input_array.partitions()(rp, cp).get_id()),
                input_array.partitions()(rp, cp).shape());

        }
    }

    // Latitudinal sides halo partitions
    //     +---+---+
    //     | W | E |
    //     +---+---+
    //     | W | E |
    //     +---+---+
    //     | W | E |
    //     +---+---+
    InputPartitions halo_latitudinal_sides_partitions{
        Shape{{nr_partitions0, 2}}};

    for(Index rp = 0; rp < nr_partitions0; ++rp) {

        for(auto const [ch, cp]: {
                std::array<Index, 2>{{0, 0}},
                std::array<Index, 2>{{1, nr_partitions1 - 1}}}) {

            halo_latitudinal_sides_partitions(rp, ch) = hpx::dataflow(
                hpx::launch::async,
                hpx::util::unwrapping(

                    [kernel_radius](
                        hpx::id_type const locality_id,
                        Shape const& partition_shape)
                    {
                        return InputPartition{locality_id,
                            Shape{{partition_shape[0], kernel_radius}}, 0};
                    }

                ),

                hpx::get_colocation_id(
                    input_array.partitions()(rp, cp).get_id()),
                input_array.partitions()(rp, cp).shape());

        }
    }

    // -------------------------------------------------------------------------
    assert(nr_partitions0 > 0);
    assert(nr_partitions1 > 0);

    InputPartitions local_input_partitions{Shape{{3, 3}}};

    // North-west corner partition
    {
        // This block also handles the first partition in case there
        // is only a single row and/or a single column of partitions

        local_input_partitions(0, 0) = halo_corner_partitions(0, 0);
        local_input_partitions(0, 1) = halo_longitudinal_side_partitions(0, 0);
        local_input_partitions(0, 2) = nr_partitions1 == 1
            ? halo_corner_partitions(0, 1)
            : halo_longitudinal_side_partitions(0, 1);

        local_input_partitions(1, 0) = halo_latitudinal_sides_partitions(0, 0);
        local_input_partitions(1, 1) = input_array.partitions()(0, 0);
        local_input_partitions(1, 2) = nr_partitions1 == 1
            ? halo_latitudinal_sides_partitions(0, 1)
            : input_array.partitions()(0, 1);

        local_input_partitions(2, 0) = nr_partitions0 == 1
            ? halo_corner_partitions(1, 0)
            : halo_latitudinal_sides_partitions(1, 0);
        local_input_partitions(2, 1) = nr_partitions0 == 1
            ? halo_longitudinal_side_partitions(1, 0)
            : input_array.partitions()(1, 0);
        local_input_partitions(2, 2) = nr_partitions0 == 1
            ? (nr_partitions1 == 1
                ? halo_corner_partitions(1, 1)
                : halo_longitudinal_side_partitions(1, 1))
            : (nr_partitions1 == 1
                ? halo_latitudinal_sides_partitions(1, 1)
                : input_array.partitions()(1, 1));

        // Once all needed partitions are ready, call the
        // remote action
        output_partitions(0, 0) = hpx::when_all_n(
                local_input_partitions.begin(),
                local_input_partitions.nr_elements()).then(
            hpx::util::unwrapping(

                [action, kernel](
                    auto&& partitions)
                {
                    InputPartitions local_input_partitions{
                        Shape{{3, 3}},
                        partitions.begin(), partitions.end()};

                    return action(
                        hpx::get_colocation_id(
                            hpx::launch::sync,
                            local_input_partitions(1, 1).get_id()),
                        local_input_partitions,
                        kernel);
                }

            ));
    }

    // North-east corner partition
    if(nr_partitions1 > 1) {
        // This block also handles the last partition in case there
        // is only a single row of partitions

        local_input_partitions(0, 0) =
            halo_longitudinal_side_partitions(0, nr_partitions1 - 2);
        local_input_partitions(0, 1) =
            halo_longitudinal_side_partitions(0, nr_partitions1 - 1);
        local_input_partitions(0, 2) = halo_corner_partitions(0, 1);

        local_input_partitions(1, 0) =
            input_array.partitions()(0, nr_partitions1 - 2);
        local_input_partitions(1, 1) =
            input_array.partitions()(0, nr_partitions1 - 1);
        local_input_partitions(1, 2) = halo_latitudinal_sides_partitions(0, 1);

        if(nr_partitions0 == 1) {
            // Case where nr_partitions1 == 1 is handled by north-west
            // corner logic
            assert(nr_partitions1 > 1);

            local_input_partitions(2, 0) =
                halo_longitudinal_side_partitions(1, nr_partitions1 - 2);
            local_input_partitions(2, 1) =
                halo_longitudinal_side_partitions(1, nr_partitions1 - 1);
            local_input_partitions(2, 2) = halo_corner_partitions(1, 1);
        }
        else {
            local_input_partitions(2, 0) =
                input_array.partitions()(1, nr_partitions1 - 2);
            local_input_partitions(2, 1) =
                input_array.partitions()(1, nr_partitions1 - 1);
            local_input_partitions(2, 2) =
                halo_latitudinal_sides_partitions(1, 1);
        }

        // Once all needed partitions are ready, call the
        // remote action
        output_partitions(0, nr_partitions1 - 1) = hpx::when_all_n(
                local_input_partitions.begin(),
                local_input_partitions.nr_elements()).then(
            hpx::util::unwrapping(

                [action, kernel](
                    auto&& partitions)
                {
                    InputPartitions local_input_partitions{
                        Shape{{3, 3}},
                        partitions.begin(), partitions.end()};

                    return action(
                        hpx::get_colocation_id(
                            hpx::launch::sync,
                            local_input_partitions(1, 1).get_id()),
                        local_input_partitions,
                        kernel);
                }

            ));
    }

    // South-west corner partition
    if(nr_partitions0 > 1) {
        // This block also handles the last partition in case there
        // is only a single column of partitions

        local_input_partitions(0, 0) =
            halo_latitudinal_sides_partitions(nr_partitions0 - 2, 0);
        local_input_partitions(0, 1) =
            input_array.partitions()(nr_partitions0 - 2, 0);
        local_input_partitions(0, 2) = nr_partitions1 == 1
            ? halo_latitudinal_sides_partitions(nr_partitions0 - 2, 1)
            : input_array.partitions()(nr_partitions0 - 2, 1);

        local_input_partitions(1, 0) =
            halo_latitudinal_sides_partitions(nr_partitions0 - 1, 0);
        local_input_partitions(1, 1) =
            input_array.partitions()(nr_partitions0 - 1, 0);
        local_input_partitions(1, 2) = nr_partitions1 == 1
            ? halo_latitudinal_sides_partitions(nr_partitions0 - 1, 1)
            : input_array.partitions()(nr_partitions0 - 1, 1);

        local_input_partitions(2, 0) = halo_corner_partitions(1, 0);
        local_input_partitions(2, 1) = halo_longitudinal_side_partitions(1, 0);
        local_input_partitions(2, 2) = nr_partitions1 == 1
            ? halo_corner_partitions(1, 1)
            : halo_longitudinal_side_partitions(1, 1);

        // Once all needed partitions are ready, call the
        // remote action
        output_partitions(nr_partitions0 - 1, 0) = hpx::when_all_n(
                local_input_partitions.begin(),
                local_input_partitions.nr_elements()).then(
            hpx::util::unwrapping(

                [action, kernel](
                    auto&& partitions)
                {
                    InputPartitions local_input_partitions{
                        Shape{{3, 3}},
                        partitions.begin(), partitions.end()};

                    return action(
                        hpx::get_colocation_id(
                            hpx::launch::sync,
                            local_input_partitions(1, 1).get_id()),
                        local_input_partitions,
                        kernel);
                }

            ));
    }

    // South-east corner partition
    if(nr_partitions0 > 1 && nr_partitions1 > 1) {
        local_input_partitions(0, 0) = input_array.partitions()(nr_partitions0 - 2, nr_partitions1 - 2);
        local_input_partitions(0, 1) = input_array.partitions()(nr_partitions0 - 2, nr_partitions1 - 1);
        local_input_partitions(0, 2) = halo_latitudinal_sides_partitions(nr_partitions0 - 2, 1);
        local_input_partitions(1, 0) = input_array.partitions()(nr_partitions0 - 1, nr_partitions1 - 2);
        local_input_partitions(1, 1) = input_array.partitions()(nr_partitions0 - 1, nr_partitions1 - 1);
        local_input_partitions(1, 2) = halo_latitudinal_sides_partitions(nr_partitions0 - 1, 1);
        local_input_partitions(2, 0) = halo_longitudinal_side_partitions(1, nr_partitions1 - 2);
        local_input_partitions(2, 1) = halo_longitudinal_side_partitions(1, nr_partitions1 - 1);
        local_input_partitions(2, 2) = halo_corner_partitions(1, 1);

        // Once all needed partitions are ready, call the
        // remote action
        output_partitions(nr_partitions0 - 1, nr_partitions1 - 1) = hpx::when_all_n(
                local_input_partitions.begin(),
                local_input_partitions.nr_elements()).then(
            hpx::util::unwrapping(

                [action, kernel](
                    auto&& partitions)
                {
                    InputPartitions local_input_partitions{
                        Shape{{3, 3}},
                        partitions.begin(), partitions.end()};

                    return action(
                        hpx::get_colocation_id(
                            hpx::launch::sync,
                            local_input_partitions(1, 1).get_id()),
                        local_input_partitions,
                        kernel);
                }

            ));
    }


    // North side partition
    {
        // This block also handles the middle partitions in
        // case there is only a single row of partitions

        for(Index c = 1; c < nr_partitions1 - 1; ++c) {
            local_input_partitions(0, 0) =
                halo_longitudinal_side_partitions(0, c - 1);
            local_input_partitions(0, 1) =
                halo_longitudinal_side_partitions(0, c    );
            local_input_partitions(0, 2) =
                halo_longitudinal_side_partitions(0, c + 1);

            local_input_partitions(1, 0) = input_array.partitions()(0, c - 1);
            local_input_partitions(1, 1) = input_array.partitions()(0, c    );
            local_input_partitions(1, 2) = input_array.partitions()(0, c + 1);

            if(nr_partitions0 == 1) {
                // Case where nr_partitions1 == 1 is handled by north-west
                // corner logic
                assert(nr_partitions1 > 1);

                local_input_partitions(2, 0) =
                    halo_longitudinal_side_partitions(1, c - 1);
                local_input_partitions(2, 1) =
                    halo_longitudinal_side_partitions(1, c    );
                local_input_partitions(2, 2) =
                    halo_longitudinal_side_partitions(1, c + 1);
            }
            else {
                local_input_partitions(2, 0) =
                    input_array.partitions()(1, c - 1);
                local_input_partitions(2, 1) =
                    input_array.partitions()(1, c    );
                local_input_partitions(2, 2) =
                    input_array.partitions()(1, c + 1);
            }

            // Once all needed partitions are ready, call the
            // remote action
            output_partitions(0, c) = hpx::when_all_n(
                    local_input_partitions.begin(),
                    local_input_partitions.nr_elements()).then(
                hpx::util::unwrapping(

                    [action, kernel](
                        auto&& partitions)
                    {
                        InputPartitions local_input_partitions{
                            Shape{{3, 3}},
                            partitions.begin(), partitions.end()};

                        return action(
                            hpx::get_colocation_id(
                                hpx::launch::sync,
                                local_input_partitions(1, 1).get_id()),
                            local_input_partitions,
                            kernel);
                    }

                ));

        }
    }

    // South side partition
    if(nr_partitions0 > 1) {

        for(Index c = 1; c < nr_partitions1 - 1; ++c) {
            local_input_partitions(0, 0) = input_array.partitions()(nr_partitions0 - 2, c - 1);
            local_input_partitions(0, 1) = input_array.partitions()(nr_partitions0 - 2, c    );
            local_input_partitions(0, 2) = input_array.partitions()(nr_partitions0 - 2, c + 1);
            local_input_partitions(1, 0) = input_array.partitions()(nr_partitions0 - 1, c - 1);
            local_input_partitions(1, 1) = input_array.partitions()(nr_partitions0 - 1, c    );
            local_input_partitions(1, 2) = input_array.partitions()(nr_partitions0 - 1, c + 1);
            local_input_partitions(2, 0) = halo_longitudinal_side_partitions(0, c - 1);
            local_input_partitions(2, 1) = halo_longitudinal_side_partitions(0, c    );
            local_input_partitions(2, 2) = halo_longitudinal_side_partitions(0, c + 1);

            // Once all needed partitions are ready, call the
            // remote action
            output_partitions(nr_partitions0 - 1, c) = hpx::when_all_n(
                    local_input_partitions.begin(),
                    local_input_partitions.nr_elements()).then(
                hpx::util::unwrapping(

                    [action, kernel](
                        auto&& partitions)
                    {
                        InputPartitions local_input_partitions{
                            Shape{{3, 3}},
                            partitions.begin(), partitions.end()};

                        return action(
                            hpx::get_colocation_id(
                                hpx::launch::sync,
                                local_input_partitions(1, 1).get_id()),
                            local_input_partitions,
                            kernel);
                    }

                ));

        }
    }

    // West side partition
    {
        // This block also handles the middle partitions in
        // case there is only a single column of partitions

        for(Index r = 1; r < nr_partitions0 - 1; ++r) {
            local_input_partitions(0, 0) =
                halo_latitudinal_sides_partitions(r - 1, 0);
            local_input_partitions(0, 1) =
                input_array.partitions()(r - 1, 0);
            local_input_partitions(0, 2) = nr_partitions1 == 1
                ? halo_latitudinal_sides_partitions(r - 1, 1)
                : input_array.partitions()(r - 1, 1);

            local_input_partitions(1, 0) =
                halo_latitudinal_sides_partitions(r    , 0);
            local_input_partitions(1, 1) =
                input_array.partitions()(r    , 0);
            local_input_partitions(1, 2) = nr_partitions1 == 1
                ? halo_latitudinal_sides_partitions(r    , 1)
                : input_array.partitions()(r    , 1);

            local_input_partitions(2, 0) =
                halo_latitudinal_sides_partitions(r + 1, 0);
            local_input_partitions(2, 1) =
                input_array.partitions()(r + 1, 0);
            local_input_partitions(2, 2) = nr_partitions1 == 1
                ? halo_latitudinal_sides_partitions(r + 1, 1)
                : input_array.partitions()(r + 1, 1);

            // Once all needed partitions are ready, call the
            // remote action
            output_partitions(r, 0) = hpx::when_all_n(
                    local_input_partitions.begin(),
                    local_input_partitions.nr_elements()).then(
                hpx::util::unwrapping(

                    [action, kernel](
                        auto&& partitions)
                    {
                        InputPartitions local_input_partitions{
                            Shape{{3, 3}},
                            partitions.begin(), partitions.end()};

                        return action(
                            hpx::get_colocation_id(
                                hpx::launch::sync,
                                local_input_partitions(1, 1).get_id()),
                            local_input_partitions,
                            kernel);
                    }

                ));

        }
    }

    // East side partition
    if(nr_partitions1 > 1) {

        for(Index r = 1; r < nr_partitions0 - 1; ++r) {
            local_input_partitions(0, 0) =
                input_array.partitions()(r - 1, nr_partitions1 - 2);
            local_input_partitions(0, 1) =
                input_array.partitions()(r - 1, nr_partitions1 - 1);
            local_input_partitions(0, 2) =
                halo_latitudinal_sides_partitions(r - 1, 1);
            local_input_partitions(1, 0) =
                input_array.partitions()(r    , nr_partitions1 - 2);
            local_input_partitions(1, 1) =
                input_array.partitions()(r    , nr_partitions1 - 1);
            local_input_partitions(1, 2) =
                halo_latitudinal_sides_partitions(r    , 1);
            local_input_partitions(2, 0) =
                input_array.partitions()(r + 1, nr_partitions1 - 2);
            local_input_partitions(2, 1) =
                input_array.partitions()(r + 1, nr_partitions1 - 1);
            local_input_partitions(2, 2) =
                halo_latitudinal_sides_partitions(r + 1, 1);

            // Once all needed partitions are ready, call the
            // remote action
            output_partitions(r, nr_partitions1 - 1) = hpx::when_all_n(
                    local_input_partitions.begin(),
                    local_input_partitions.nr_elements()).then(
                hpx::util::unwrapping(

                    [action, kernel](
                        auto&& partitions)
                    {
                        InputPartitions local_input_partitions{
                            Shape{{3, 3}},
                            partitions.begin(), partitions.end()};

                        return action(
                            hpx::get_colocation_id(
                                hpx::launch::sync,
                                local_input_partitions(1, 1).get_id()),
                            local_input_partitions,
                            kernel);
                    }

                ));

        }
    }

    // Iterate over inner partitions
    for(Index r = 1; r < nr_partitions0 - 1; ++r) {
        for(Index c = 1; c < nr_partitions1 - 1; ++c) {
            // Create collection of partitions containing the current
            // partition and its neighboring partitions
            {
                // Assume one neighboring partition is enough
                Count const radius = 1;

                for(Index i = 0; i < 2 * radius + 1; ++i) {
                    for(Index j = 0; j < 2 * radius + 1; ++j) {
                        local_input_partitions(i, j) =
                            input_array.partitions()(
                                r - radius + i, c - radius + j);
                    }
                }
            }

            // Once all needed partitions are ready, call the
            // remote action
            output_partitions(r, c) = hpx::when_all_n(
                    local_input_partitions.begin(),
                    local_input_partitions.nr_elements()).then(
                hpx::util::unwrapping(

                    [action, kernel](
                        auto&& partitions)
                    {
                        InputPartitions local_input_partitions{
                            Shape{{3, 3}},
                            partitions.begin(), partitions.end()};

                        return action(
                            hpx::get_colocation_id(
                                hpx::launch::sync,
                                local_input_partitions(1, 1).get_id()),
                            local_input_partitions,
                            kernel);
                    }

                ));
        }
    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}


template<
    typename Array,
    typename Kernel,
    typename OutputElement>
class Convolution
{

    using OutputArray = PartitionedArrayT<Array, OutputElement>;

    static constexpr Rank rank = lue::rank<Array>;

    static_assert(rank == 1 || rank == 2);

public:

    Convolution(
            Array const& array,
            Kernel const& kernel):

        _array{array},
        _kernel{kernel}

    {
    }

    OutputArray operator()() const
    {
        if constexpr(rank == 1) {
            // return convolve_1d<Array, Kernel, OutputElement>(_array, _kernel);
        }
        else if constexpr(rank == 2) {
            return convolve_2d<Array, Kernel, OutputElement>(_array, _kernel);
        }
    }

private:

    Array const&   _array;

    Kernel const&  _kernel;

};





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
    typename Kernel,
    typename OutputElement=double>
PartitionedArrayT<Array, OutputElement> convolve(
    Array const& array,
    Kernel const& kernel)
{
    detail::Convolution<Array, Kernel, OutputElement> convolution{
        array, kernel};

    return convolution();

    // convolution.start();

    // return std::move(convolution).result();
}


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
    // called multiple times?


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


    // return detail::convolve(array, kernel);


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



    // Iterate over all partitions and call action that calculates the
    // result for each partition. This action must receive the focus
    // partition, and the partitions surrounding it.


    // -------------------------------------------------------------------------
    // Inner partitions:

    // 1D: act(locality_id,
    //     left_partition, focus_partition, right_partition
    // );

    // 2D: act(locality_id,
    //     top_left_partition, top_partition, top_right_partition,
    //         left_partition, focus_partition, right_partition,
    //     bottom_left_partition, bottom_partition, bottom_right_partition
    // );

    // 3D: act(locality_id,
    //     ...
    // );


    // -------------------------------------------------------------------------
    // Corner partitions:


    // -------------------------------------------------------------------------
    // Border partitions:

    // return detail::convolve(array, kernel);



}  // namespace lue
