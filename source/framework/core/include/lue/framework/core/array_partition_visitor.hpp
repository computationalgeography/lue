#pragma once
#include "lue/framework/core/array_visitor.hpp"


namespace lue {

template<
    typename Partitions>
class PartitionVisitor:
    public ArrayVisitor<ShapeT<Partitions>>
{

private:

    using Base = ArrayVisitor<ShapeT<Partitions>>;

public:

    PartitionVisitor(
        Partitions& partitions):

        Base{partitions.shape()},
        _partitions{partitions}

    {
    }

protected:

    PartitionT<Partitions>& partition()
    {
        assert(this->cursor().linear_idx() < _partitions.size());

        return _partitions[this->cursor().linear_idx()];
    }

private:

    //! Array containing partitions being visited
    Partitions&    _partitions;

};


template<
    typename Partitions>
class ConstPartitionVisitor:
    public ArrayVisitor<ShapeT<Partitions>>
{

private:

    using Base = ArrayVisitor<ShapeT<Partitions>>;

public:

    ConstPartitionVisitor(
        Partitions const& partitions):

        Base{partitions.shape()},
        _partitions{partitions}

    {
    }

protected:

    PartitionT<Partitions> const& partition() const
    {
        assert(this->cursor().linear_idx() < _partitions.size());

        return _partitions[this->cursor().linear_idx()];
    }

private:

    //! Array containing partitions being visited
    Partitions const& _partitions;

};

}  // namespace lue
