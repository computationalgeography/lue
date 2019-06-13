#pragma once
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/core/array_partition_definition.hpp"
// #include <boost/multi_array.hpp>
#include <experimental/iterator>


// namespace boost {
// 
// template<
//     typename Element,
//     std::size_t rank,
//     typename Allocator>
// std::ostream& operator<<(
//     std::ostream& stream,
//     multi_array<Element, rank, Allocator> const& array)
// {
//     stream << '[';
//     auto joiner = std::experimental::make_ostream_joiner(stream, ", ");
// 
//     auto const size = array.num_elements();
//     auto const begin = array.data();
//     auto const end = array.data() + size;
// 
//     // Max number of values to print at start and end
//     std::size_t const halo = 5;
// 
//     if(size <= 2 * halo) {
//         // Print all values
//         std::copy(begin, end, joiner);
//     }
//     else {
//         // Print first and last halo number of values
//         std::copy(begin, begin + halo, joiner);
//         stream << ", ..., ";
//         std::copy(end - halo, end, joiner);
//     }
//     stream << ']';
// 
//     return stream;
// }
// 
// }  // namespace boost


namespace std {

template<
    typename T,
    std::size_t rank>
std::ostream& operator<<(
    std::ostream& stream,
    std::array<T, rank> const& array)
{
    stream << '[';
    std::copy(
        std::begin(array), std::end(array),
        std::experimental::make_ostream_joiner(stream, ", "));

        // Prints a separator after the last element...
        // std::ostream_iterator<Index>(stream, ", "));
    stream << ']';

    return stream;
}

}  // namespace std


namespace lue {

template<
    typename Index,
    std::size_t rank>
std::ostream& operator<<(
    std::ostream& stream,
    ArrayPartitionDefinition<Index, rank> const& definition)
{
    stream <<
        "start: " << definition.start() << ", "
        "shape: " << definition.shape();

    return stream;
}


template<
    typename Element,
    std::size_t rank>
std::ostream& operator<<(
    std::ostream& stream,
    ArrayPartitionData<Element, rank> const& data)
{
    stream
        << data.shape()
        << ", "
        << "[\n";

    auto joiner = std::experimental::make_ostream_joiner(stream, ", ");

    auto const size = data.size();
    auto const begin = data.begin();
    auto const end = data.end();

    // Max number of values to print at start and end
    std::size_t const halo = 5;

    if(size <= 2 * halo) {
        // Print all values
        std::copy(begin, end, joiner);
    }
    else {
        // Print first and last halo number of values
        std::copy(begin, begin + halo, joiner);
        stream << ", ..., ";
        std::copy(end - halo, end, joiner);
    }

    stream << "]\n";

    return stream;
}


template<
    typename Element,
    std::size_t rank>
std::ostream& operator<<(
    std::ostream& stream,
    ArrayPartition<Element, rank> const& partition)
{
    stream
        << "    shape     : " << partition.shape().get() << "\n"
        << "    size      : " << partition.size().get() << "\n"
        << partition.data().get()
        ;

    return stream;
}


template<
    typename Element,
    std::size_t rank>
std::ostream& operator<<(
    std::ostream& stream,
    PartitionedArray<Element, rank> const& array)
{
    stream
        << "nr_partitions: " << array.nr_partitions() << "\n"
        << "nr_elements  : " << array.nr_elements() << "\n"
        << "shape        : " << array.shape() << "\n"
        << array.partitions()
        ;

    return stream;
}

}  // namespace lue
