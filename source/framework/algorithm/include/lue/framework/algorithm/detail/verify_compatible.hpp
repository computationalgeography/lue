#pragma once
#include "lue/framework/partitioned_array_decl.hpp"
#include "lue/string.hpp"
#include <format>
#include <stdexcept>


namespace lue::detail {

    /*!
        @overload
    */
    template<typename Element, Rank rank>
    void verify_compatible([[maybe_unused]] PartitionedArray<Element, rank> const& array)
    {
    }


    /*!
        @overload
    */
    template<typename Element1, typename Element2, Rank rank>
    void verify_compatible(
        PartitionedArray<Element1, rank> const& array1, PartitionedArray<Element2, rank> const& array2)
    {
        if (array1.shape() != array2.shape())
        {
            throw std::runtime_error(
                std::format(
                    "Shapes differ: {} vs {}", join(array1.shape(), ", "), join(array2.shape(), ", ")));
        }

        if (array1.partitions().shape() != array2.partitions().shape())
        {
            throw std::runtime_error(
                std::format(
                    "Shapes in partitions differ: {} vs {}",
                    join(array1.partitions().shape(), ", "),
                    join(array2.partitions().shape(), ", ")));
        }
    }


    /*!
        @brief      Verify whether the arrays passed in are compatible with the first one
        @tparam     Element Type of array elements
        @tparam     rank Rank of the array
        @return     .
        @exception  std::runtime_error When one of the arrays is not compatible

        Compatible arrays have the same domain decomposition:

        - Shape is the same
        - Shape in partitions is the same
        - Localities of the partitions are the same
    */
    template<typename Element1, typename Element2, typename... Elements, Rank rank>
    void verify_compatible(
        PartitionedArray<Element1, rank> const& array1,
        PartitionedArray<Element2, rank> const& array2,
        PartitionedArray<Elements, rank> const&... arrays)
    {
        verify_compatible(array1, array2);
        verify_compatible(array2, arrays...);
    }

}  // namespace lue::detail
