#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include "lue/framework.hpp"


namespace lue {

    /*!
        @brief      Type for the classes used by the accu_info algorithm

        Possible values are:

        - ridge_cell
        - intra_partition_stream_cell
        - partition_output_cell
        - partition_input_cell
        - partition_input_output_cell
        - inter_partition_stream_cell
        - sink_cell
    */
    using CellClass = SmallestUnsignedIntegralElement;

    /*!
        @brief      Cells without upstream neighbours
    */
    static constexpr CellClass ridge_cell = 11;

    /*!
        @brief      Cell whose upstream neighbours are all located within the same partition
    */
    static constexpr CellClass intra_partition_stream_cell = 12;

    /*!
        @brief      Cell with downstream neighbour in other partition
    */
    static constexpr CellClass partition_output_cell = 13;

    /*!
        @brief      Cell with upstream neighbour in other partition
    */
    static constexpr CellClass partition_input_cell = 14;

    /*!
        @brief      Cell with upstream and downstream neighbour in other partition
    */
    static constexpr CellClass partition_input_output_cell = 15;

    /*!
        @brief      Cell with at least one upstream neighbour that is located in another partition
    */
    static constexpr CellClass inter_partition_stream_cell = 16;

    /*!
        @brief      Cell without downstream neighbours
    */
    static constexpr CellClass sink_cell = 17;


    /*!
        @brief      Cell with multiple upstream neighbours
    */
    static constexpr CellClass confluence_cell = 18;


    /*!
        @brief      Classify cells according to their position within the @a flow_direction field and within
                    array partitions
        @return     Array with for each valid input cell a class of type CellClass

        This algorithm is useful to illustrate the workings of the underlying flow accumulation algorithm
        used by some of the routing operations.
    */
    template<typename Policies>
    auto accu_info(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;

}  // namespace lue
