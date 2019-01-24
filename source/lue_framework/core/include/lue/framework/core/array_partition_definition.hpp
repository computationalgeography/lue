#pragma once
#include "lue/framework/core/shape.hpp"


namespace lue {

template<
    typename Index_,
    std::size_t rank>
class ArrayPartitionDefinition
{

public:

    using Index = Index_;

    using Start = lue::Shape<Index, rank>;

    using Shape = lue::Shape<Index, rank>;

                   ArrayPartitionDefinition();

                   ArrayPartitionDefinition(
                                        Shape const& shape);

                   ArrayPartitionDefinition(
                                        Start const& start,
                                        Shape const& shape);

                   ArrayPartitionDefinition (
                                    ArrayPartitionDefinition const&)=default;

                   ArrayPartitionDefinition (
                                    ArrayPartitionDefinition&&)=default;

                   ~ArrayPartitionDefinition()=default;

    ArrayPartitionDefinition& operator=(
                                    ArrayPartitionDefinition const&)=default;

    ArrayPartitionDefinition& operator=(
                                    ArrayPartitionDefinition&&)=default;

    bool           operator==          (
                                ArrayPartitionDefinition const& other) const;

    Start const&   start               () const;

    Start&         start               ();

    Shape const&   shape               () const;

    Shape&         shape               ();

    Index          nr_elements         () const;

private:

    Start          _start;

    Shape          _shape;

};


template<
    typename Index,
    std::size_t rank>
inline ArrayPartitionDefinition<Index, rank>::ArrayPartitionDefinition():

    _start{},
    _shape{}

{
}


template<
    typename Index,
    std::size_t rank>
inline ArrayPartitionDefinition<Index, rank>::ArrayPartitionDefinition(
    Shape const& shape):

    _start{},
    _shape{shape}

{
}


template<
    typename Index,
    std::size_t rank>
inline ArrayPartitionDefinition<Index, rank>::ArrayPartitionDefinition(
    Start const& start,
    Shape const& shape):

    _start{start},
    _shape{shape}

{
}


template<
    typename Index,
    std::size_t rank>
inline bool ArrayPartitionDefinition<Index, rank>::operator==(
    ArrayPartitionDefinition const& other) const
{
    return _start == other._start && _shape == other._shape;
}


template<
    typename Index,
    std::size_t rank>
inline typename ArrayPartitionDefinition<Index, rank>::Start const&
    ArrayPartitionDefinition<Index, rank>::start() const
{
    return _start;
}


template<
    typename Index,
    std::size_t rank>
inline typename ArrayPartitionDefinition<Index, rank>::Start&
    ArrayPartitionDefinition<Index, rank>::start()
{
    return _start;
}


template<
    typename Index,
    std::size_t rank>
inline typename ArrayPartitionDefinition<Index, rank>::Shape const&
    ArrayPartitionDefinition<Index, rank>::shape() const
{
    return _shape;
}


template<
    typename Index,
    std::size_t rank>
inline typename ArrayPartitionDefinition<Index, rank>::Shape&
    ArrayPartitionDefinition<Index, rank>::shape()
{
    return _shape;
}


template<
    typename Index,
    std::size_t rank>
inline Index ArrayPartitionDefinition<Index, rank>::nr_elements() const
{
    return lue::nr_elements(_shape);
}

}  // namespace lue
