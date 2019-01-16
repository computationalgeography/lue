#pragma once
#include <experimental/iterator>


namespace lue {

template<
    typename Index,
    std::size_t rank>
std::ostream& operator<<(
    std::ostream& stream,
    Shape<Index, rank> const& shape)
{
    stream << '[';
    std::copy(
        std::begin(shape), std::end(shape),
        std::experimental::make_ostream_joiner(stream, ", "));

        // Prints a separator after the last element...
        // std::ostream_iterator<Index>(stream, ", "));
    stream << ']';

    return stream;
}


template<
    typename Index,
    std::size_t rank>
std::ostream& operator<<(
    std::ostream& stream,
    PartitionDefinition<Index, rank> const& definition)
{
    stream <<
        "start: " << definition.start() << ", shape: " << definition.shape();

    return stream;
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
std::ostream& operator<<(
    std::ostream& stream,
    ArrayPartitionData<Index, Value, rank> const& data)
{
    stream << data.definition() << ", [";

    auto joiner = std::experimental::make_ostream_joiner(stream, ", ");

    auto const size = data.values().num_elements();
    auto const begin = data.values().data();
    auto const end = data.values().data() + size;

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

    stream << ']';

    return stream;
}

}  // namespace lue
