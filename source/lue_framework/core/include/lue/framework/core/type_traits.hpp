#pragma once


namespace lue {

template<
    typename ArrayPartitionData>
class ArrayPartitionDataTypeTraits
{
};


// Default array partition type traits
template<
    typename ArrayPartition>
class ArrayPartitionTypeTraits
{
};


template<
    typename ArrayPartitions>
class ArrayPartitionsTypeTraits
{
};


// core/array_type_traits.hpp
// Default array type traits
template<
    typename PartitionedArray>
class PartitionedArrayTypeTraits
{
};

}  // namespace lue
