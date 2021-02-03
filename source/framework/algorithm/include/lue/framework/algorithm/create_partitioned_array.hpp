#pragma once
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {
namespace detail {

class LocalityIdxByPartitionIdx
{

    public:

        LocalityIdxByPartitionIdx(
            Index const nr_partitions,
            Index const nr_localities):

            _nr_partitions{nr_partitions},
            _nr_localities{nr_localities}

        {
            lue_assert(_nr_partitions > 0);
            lue_assert(_nr_localities > 0);
        }

        Index operator()(
            Index const partition_idx) const
        {
            return map_to_range(
                Index{0}, _nr_partitions - 1,
                Index{0}, _nr_localities - 1,
                partition_idx);
        }

    private:

        Index _nr_partitions;

        Index _nr_localities;

};


template<
    typename Policies,
    Rank rank,
    typename Functor>
class InstantiatePartitionsBase
{

    public:

        using OutputElement = OutputElementT<Functor>;
        using Array = PartitionedArray<OutputElement, rank>;
        using Partitions = PartitionsT<Array>;
        using Offset = OffsetT<Array>;
        using Shape = ShapeT<Array>;
        using Localities = lue::Localities<rank>;

        InstantiatePartitionsBase(
            [[maybe_unused]] Policies const& policies,
            Shape const& shape_in_partitions,
            Shape const& partition_shape,
            Functor const& partition_creator,
            std::vector<hpx::id_type>&& localities):

            _localities{shape_in_partitions},
            _partitions{shape_in_partitions},
            _partition_shape{partition_shape},
            _partition_creator{partition_creator},
            _locality_idx_by_partition_idx{nr_elements(_partitions), Index(localities.size())},
            _localities_vector{std::forward<std::vector<hpx::id_type>>(localities)}

        {
            lue_assert(std::none_of(_localities.begin(), _localities.end(),
                [](hpx::id_type const locality_id)
                {
                    return bool{locality_id};
                }));
            lue_assert(std::all_of(_localities_vector.begin(), _localities_vector.end(),
                [](hpx::id_type const locality_id)
                {
                    return bool{locality_id};
                }));
        }


        Localities localities()
        {
            // Sink return. Call this method only once...
            lue_assert(std::all_of(_localities.begin(), _localities.end(),
                [](hpx::id_type const locality_id)
                {
                    return bool{locality_id};
                }));
            return std::move(_localities);
        }


        Partitions partitions()
        {
            // Sink return. Call this method only once...
            return std::move(_partitions);
        }

        void assert_invariants() const
        {
            lue_assert(_partitions.shape() == _localities.shape());

            lue_assert(std::all_of(_localities.begin(), _localities.end(),
                [](hpx::id_type const locality_id)
                {
                    return bool{locality_id};
                }));
        }

    protected:

        hpx::id_type locality_id(
            Index const partition_idx)  // Along curve!
        {
            // partition_idx is an index along a curve visiting all
            // partitions
            return _localities_vector[_locality_idx_by_partition_idx(partition_idx)];
        }

        template<
            typename... Idxs>
        Offset offset(
            Idxs... partition_idxs)  // In array!
        {
            Offset offset{partition_idxs...};

            for(std::size_t d = 0; d < rank; ++d)
            {
                offset[d] *= _partition_shape[d];
            }

            return offset;
        }

        std::size_t nr_partitions() const
        {
            return nr_elements(_partitions);
        }

    private:

    protected:

        Localities _localities;

        Partitions _partitions;

        Shape _partition_shape;

        Functor _partition_creator;

    private:

        LocalityIdxByPartitionIdx _locality_idx_by_partition_idx;

        std::vector<hpx::id_type> _localities_vector;

};


template<
    typename Policies,
    Rank rank,
    typename Functor>
class InstantiatePartition:
    public InstantiatePartitionsBase<Policies, rank, Functor>
{

    public:

        using Base = InstantiatePartitionsBase<Policies, rank, Functor>;

        InstantiatePartition(
            Policies const& policies,
            typename Base::Shape const& shape_in_partitions,
            typename Base::Shape const& partition_shape,
            Functor const& partition_creator,
            std::vector<hpx::id_type>&& localities):

            Base{policies, shape_in_partitions, partition_shape, partition_creator,
                std::forward<std::vector<hpx::id_type>>(localities)}

        {
        }


        template<
            typename... Idxs>
        void operator()(
            Index const partition_idx,  // Along curve!
            Idxs... partition_idxs)  // In array!
        {
            hpx::id_type const locality_id{this->locality_id(partition_idx)};
            this->_localities(partition_idxs...) = locality_id;
            this->_partitions(partition_idxs...) =
                this->_partition_creator.instantiate(
                    locality_id, this->offset(partition_idxs...), this->_partition_shape);
        }

    private:

};


template<
    typename Policies,
    Rank rank,
    typename Functor>
class InstantiatePartitions:
    public InstantiatePartitionsBase<Policies, rank, Functor>
{

    public:

        using Base = InstantiatePartitionsBase<Policies, rank, Functor>;

        InstantiatePartitions(
            Policies const& policies,
            typename Base::Shape const& shape_in_partitions,
            typename Base::Shape const& partition_shape,
            Functor const& partition_creator,
            std::vector<hpx::id_type>&& localities):

            Base{policies, shape_in_partitions, partition_shape, partition_creator,
                std::forward<std::vector<hpx::id_type>>(localities)},
            _current_locality_id{},
            _offsets{},
            _idxs{}

        {
            lue_assert(!_current_locality_id);
            lue_assert(_offsets.empty());
            lue_assert(_idxs.empty());
        }


        template<
            typename... Idxs>
        void operator()(
            Index const partition_idx,  // Along curve!
            Idxs... partition_idxs)  // In array!
        {
            hpx::id_type const locality_id{this->locality_id(partition_idx)};
            typename Base::Offset const offset{this->offset(partition_idxs...)};

            if(!_current_locality_id)
            {
                lue_assert(_offsets.empty());
                lue_assert(_idxs.empty());
                _current_locality_id = locality_id;
            }

            lue_assert(_current_locality_id);

            if(locality_id != _current_locality_id || partition_idx == Index(this->nr_partitions()) - 1)
            {
                // Instantiate partitions for the same locality once we jump
                // to another locality, or once the partition_idx passed in
                // is the last one we will get

                // Ask for the creation of a set of partitions and assign
                // the resulting partition clients to the correct location
                // in the array.
                using Partitions = typename Base::Partitions;
                using Partition = PartitionT<Partitions>;

                hpx::future<std::vector<Partition>> partitions_f{
                    this->_partition_creator.instantiate(
                        _current_locality_id, _offsets, this->_partition_shape)};
                partitions_f.wait();

                // The collection of partitions is ready. This does not
                // imply that the partitions themselves are ready. This
                // does not matter though.

                // Assign the partitions to their correct location in the array
                {
                    std::vector<Partition> partitions{partitions_f.get()};
                    lue_assert(partitions.size() == _idxs.size());

                    for(std::size_t i = 0; i < partitions.size(); ++i)
                    {
                        std::apply(

                            [this, locality_id=_current_locality_id, partition=std::move(partitions[i])](
                                Idxs... idxs)
                            {
                                this->_localities(idxs...) = locality_id;
                                this->_partitions(idxs...) = std::move(partition);
                            },

                            _idxs[i]);
                    }
                }

                _offsets.clear();
                _idxs.clear();
            }
            else
            {
                // Do nothing while we are still instantiating for the same locality
                // Cache information necessary for instantiating this
                // specific partition.
                lue_assert(locality_id == _current_locality_id);
                _offsets.push_back(offset);
                _idxs.push_back(std::array<Index, rank>{partition_idxs...});
            }
        }

    private:

        using Offsets = std::vector<typename Base::Offset>;
        using Idxs = std::vector<std::array<Index, rank>>;

        hpx::id_type _current_locality_id;

        Offsets _offsets;

        Idxs _idxs;

};


template<
    typename Policies,
    typename Shape,
    typename Instantiator>
PartitionedArray<OutputElementT<Instantiator>, rank<Shape>> instantiate_partitions(
    Policies const& /* policies */,
    Shape const& array_shape,
    Shape const& shape_in_partitions,
    Shape const& /* partition_shape */,
    Instantiator&& instantiator)  // The instantiator is ours
    // std::vector<hpx::id_type> const& /* localities */)
{
    using Element = OutputElementT<Instantiator>;
    constexpr Rank rank{lue::rank<Shape>};
    using Array = PartitionedArray<Element, rank>;

    // Pass a reference to the instantiator to the curve functions. We
    // need access to the results of the visit.

    if constexpr(rank == 2) //  || rank == 3)
    {
        // Visit all cells in the array in an order defined by the
        // Hilbert curve. The functor will receive a linear partition
        // index (along the Hilbert curve!) and the indices of the array
        // partition.
        //
        // The indices of the array partition locate the partition to
        // be instantiated. The linear index can be used to determine
        // a locality to put the partition component on. By mapping
        // nearby linear indices to the same locality, nearby partitions
        // end up at the same locality.

        hilbert_curve<rank>(shape_in_partitions, std::ref(instantiator));
    }
    else
    {
        linear_curve<rank>(shape_in_partitions, std::ref(instantiator));
    }

#ifndef NDEBUG
    instantiator.assert_invariants();
#endif

    // All partitions are being created. Return an array containing them.
    return Array{array_shape, instantiator.localities(), instantiator.partitions()};
}


template<
    typename Policies,
    typename Shape,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank<Shape>> instantiate_partitions(
    Policies const& policies,
    Shape const& array_shape,
    Shape const& shape_in_partitions,
    Shape const& partition_shape,
    Functor const& partition_creator)
{
    // Create array containing partitions. Each of these partitions
    // will be a component client instance referring to a, possibly
    // remote, component server instance.

    using Element = OutputElementT<Functor>;
    using Array = PartitionedArray<Element, rank<Shape>>;
    using Partitions = PartitionsT<Array>;

    Partitions partitions{shape_in_partitions};
    Count const nr_partitions = lue::nr_elements(shape_in_partitions);
    lue_assert(partitions.nr_elements() == nr_partitions);

    std::vector<hpx::id_type> localities = hpx::find_all_localities();

    [[maybe_unused]] Count const nr_localities = localities.size();
    lue_assert(nr_localities > 0);

    if(!BuildOptions::build_test)
    {
        // In general, the number of localities should be smaller than
        // the number of partitions. Otherwise more hardware is used than
        // necessary. The exception is when we are building with tests
        // turned on. Tests may have to run on less localities than there
        // are partitions.
        if(nr_partitions < nr_localities)
        {
            throw std::runtime_error(fmt::format(
                "Not enough partitions to use all localities ({} < {})",
                nr_partitions, nr_localities));
        }
    }

    lue_assert(hpx::find_here() == hpx::find_root_locality());
    lue_assert(localities[0] == hpx::find_root_locality());

    if constexpr (Functor::instantiate_per_locality)
    {
        // Per locality, instantiate all partitions in one go
        InstantiatePartitions<Policies, rank<Shape>, Functor> instantiator{
            policies, shape_in_partitions, partition_shape, partition_creator, std::move(localities)};
        return instantiate_partitions(
            policies, array_shape, shape_in_partitions, partition_shape, std::move(instantiator));
    }
    else
    {
        // Instantiate each partition individually
        InstantiatePartition<Policies, rank<Shape>, Functor> instantiator{
            policies, shape_in_partitions, partition_shape, partition_creator, std::move(localities)};
        return instantiate_partitions(
            policies, array_shape, shape_in_partitions, partition_shape, std::move(instantiator));
    }
}

}  // namespace detail


namespace policy::create_partitioned_array {

    template<
        typename OutputElement>
    using DefaultPolicies = policy::DefaultPolicies<
        OutputElements<OutputElement>,
        InputElements<>>;

}  // namespace policy::create_partitioned_array


template<
    typename Policies,
    typename Shape,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank<Shape>> create_partitioned_array(
    Policies const& policies,
    Shape const& array_shape,
    Shape const& partition_shape,
    Functor const& partition_creator,
    typename PartitionedArray<OutputElementT<Functor>, rank<Shape>>::ClampMode const clamp_mode=
        PartitionedArray<OutputElementT<Functor>, rank<Shape>>::ClampMode::merge)
{
    // Create the array partitions that, together make up the partitioned
    // array. Note that the extent of this array might be too large,
    // given that we use a certain partition shape for all partitions.

    using Element = OutputElementT<Functor>;
    using Array = PartitionedArray<Element, rank<Shape>>;

    // Given the shape of the array and the shape of the array partitions,
    // determine the shape of the array in partitions

    Shape const shape_in_partitions = lue::shape_in_partitions(array_shape, partition_shape);

    Array array{detail::instantiate_partitions(
        policies, array_shape, shape_in_partitions, partition_shape, partition_creator)};

    // TODO
    switch(clamp_mode)
    {
        case Array::ClampMode::shrink:
        {
    //         // Fix too large extent by shrinking overflowing partitions. This
    //         // may result is partitions that are too small for the focal
    //         // algorithms. Therefore, the alternative approach is more robust.
    //         clamp_array_shrink(shape_in_partitions, partition_shape);
    //         break;
        }
        case Array::ClampMode::merge:
        {
    //         // Fix too large extent by merging relevant part of overflowing
    //         // partitions with bordering partitions. These will become larger
    //         // than the requested partition size. But focal algorithms will
    //         // behave more robust. Partitions in the array will be at least as
    //         // large as the partitions shape requested.
    //         clamp_array_merge(shape_in_partitions, partition_shape);
    //         break;
        }
    }

    // assert_invariants();

    return array;
}


template<
    typename Shape,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank<Shape>> create_partitioned_array(
    Shape const& array_shape,
    Shape const& partition_shape,
    Functor const& partition_creator,
    typename PartitionedArray<OutputElementT<Functor>, rank<Shape>>::ClampMode const clamp_mode=
        PartitionedArray<OutputElementT<Functor>, rank<Shape>>::ClampMode::merge)
{
    using Element = OutputElementT<Functor>;
    using Policies = policy::create_partitioned_array::DefaultPolicies<Element>;

    return create_partitioned_array(
        Policies{}, array_shape, partition_shape, partition_creator, clamp_mode);
}

}  // namespace lue
