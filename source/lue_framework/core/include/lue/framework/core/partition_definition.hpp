#pragma once
#include "lue/framework/core/shape.hpp"


namespace lue {

template<
    typename Index,
    std::size_t rank>
class PartitionDefinition
{

public:

    using Start = lue::Shape<Index, rank>;
    using Shape = lue::Shape<Index, rank>;

                   PartitionDefinition ()=default;

                   PartitionDefinition (Shape const& shape);

                   PartitionDefinition (Start const& start,
                                        Shape const& shape);

                   PartitionDefinition (PartitionDefinition const&)=default;

                   PartitionDefinition (PartitionDefinition&&)=default;

                   ~PartitionDefinition()=default;

    PartitionDefinition& operator=     (PartitionDefinition const&)=default;

    PartitionDefinition& operator=     (PartitionDefinition&&)=default;

    bool           operator==          (PartitionDefinition const& other) const;

    Start const&   start               () const;

    Start&         start               ();

    Shape const&   shape               () const;

    Shape&         shape               ();

private:

    Start          _start;

    Shape          _shape;

};


template<
    typename Index,
    std::size_t rank>
inline PartitionDefinition<Index, rank>::PartitionDefinition(
    Shape const& shape):

    _start{},
    _shape{shape}

{
}


template<
    typename Index,
    std::size_t rank>
inline PartitionDefinition<Index, rank>::PartitionDefinition(
    Start const& start,
    Shape const& shape):

    _start{start},
    _shape{shape}

{
}


template<
    typename Index,
    std::size_t rank>
inline bool PartitionDefinition<Index, rank>::operator==(
    PartitionDefinition const& other) const
{
    return _start == other._start && _shape == other._shape;
}


template<
    typename Index,
    std::size_t rank>
inline typename PartitionDefinition<Index, rank>::Start const&
    PartitionDefinition<Index, rank>::start() const
{
    return _start;
}


template<
    typename Index,
    std::size_t rank>
inline typename PartitionDefinition<Index, rank>::Start&
    PartitionDefinition<Index, rank>::start()
{
    return _start;
}


template<
    typename Index,
    std::size_t rank>
inline typename PartitionDefinition<Index, rank>::Shape const&
    PartitionDefinition<Index, rank>::shape() const
{
    return _shape;
}


template<
    typename Index,
    std::size_t rank>
inline typename PartitionDefinition<Index, rank>::Shape&
    PartitionDefinition<Index, rank>::shape()
{
    return _shape;
}

}  // namespace lue
