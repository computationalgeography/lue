#pragma once
#include "lue/framework/algorithm/detail/clamp_array.hpp"
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
        using OutputArray = PartitionedArray<OutputElement, rank>;
        using Partitions = PartitionsT<OutputArray>;
        using Offset = OffsetT<OutputArray>;
        using Shape = ShapeT<OutputArray>;
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

#ifndef NDEBUG
        void assert_invariants() const
        {
            lue_assert(_partitions.shape() == _localities.shape());

            lue_assert(std::all_of(_localities.begin(), _localities.end(),
                [](hpx::id_type const locality_id)
                {
                    return bool{locality_id};
                }));
        }
#endif

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
            Idxs const... partition_idxs)  // In array!
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
            Idxs const... partition_idxs)  // In array!
        {
            hpx::id_type const locality_id{this->locality_id(partition_idx)};
            this->_localities(partition_idxs...) = locality_id;
            this->_partitions(partition_idxs...) =
                this->_partition_creator.instantiate(
                    locality_id, partition_idx, this->offset(partition_idxs...), this->_partition_shape);
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
            Idxs const... partition_idxs)  // In array!
        {
            hpx::id_type const locality_id{this->locality_id(partition_idx)};

            if(partition_idx == 0)
            {
                prepare_for_new_locality(locality_id);
            }

            typename Base::Offset const offset{this->offset(partition_idxs...)};

            bool const same_locality{locality_id == _current_locality_id};
            bool const last_partition{partition_idx == Index(this->nr_partitions()) - 1};

            if(same_locality)
            {
                cache_partition_information(offset, partition_idxs...);
            }
            else
            {
                instantiate_partitions<Idxs...>();
                prepare_for_new_locality(locality_id);
                cache_partition_information(offset, partition_idxs...);

            }

            if(last_partition)
            {
                instantiate_partitions<Idxs...>();
            }
        }

    private:

        using Offsets = std::vector<typename Base::Offset>;
        using Idxs = std::vector<std::array<Index, rank>>;

        void prepare_for_new_locality(
            hpx::id_type const locality_id)
        {
            lue_assert(locality_id);
            _current_locality_id = locality_id;
            _offsets.clear();
            _idxs.clear();
        }

        template<
            typename... Idxs>
        void cache_partition_information(
            typename Base::Offset const& offset,
            Idxs const... partition_idxs)
        {
            // lue_assert(locality_id == _current_locality_id);
            _offsets.push_back(offset);
            _idxs.push_back(std::array<Index, rank>{partition_idxs...});
        }

        template<
            typename... Idxs>
        void instantiate_partitions()
        {
            // Ask for the creation of a set of partitions and assign
            // the resulting partition clients to the correct location
            // in the array.
            using Partitions = typename Base::Partitions;
            using Partition = PartitionT<Partitions>;

            hpx::future<std::vector<Partition>> partitions_f{
                this->_partition_creator.instantiate(
                    _current_locality_id, _offsets, this->_partition_shape)};

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
        }

        hpx::id_type _current_locality_id;

        Offsets _offsets;

        Idxs _idxs;

};


template<
    typename Policies,
    typename Shape,
    typename Instantiator>
std::tuple<
            Localities<rank<Shape>>,
            PartitionsT<PartitionedArray<OutputElementT<Instantiator>, rank<Shape>>>
        > instantiate_partitions2(
    Policies const& /* policies */,
    Shape const& shape_in_partitions,
    Shape const& /* partition_shape */,
    Instantiator&& instantiator)  // The instantiator is ours
{
    constexpr Rank rank{lue::rank<Shape>};

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
    // return OutputArray{array_shape, instantiator.localities(), instantiator.partitions()};
    return std::make_tuple(instantiator.localities(), instantiator.partitions());
}


template<
    typename Policies,
    typename Shape,
    typename Functor>
std::tuple<
            Localities<rank<Shape>>,
            PartitionsT<PartitionedArray<OutputElementT<Functor>, rank<Shape>>>
        > instantiate_partitions(
    Policies const& policies,
    Shape const& shape_in_partitions,
    Shape const& partition_shape,
    Functor const& partition_creator)
{
    // Create array containing partitions. Each of these partitions
    // will be a component client instance referring to a, possibly
    // remote, component server instance.

    using Element = OutputElementT<Functor>;
    using OutputArray = PartitionedArray<Element, rank<Shape>>;
    using Partitions = PartitionsT<OutputArray>;

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
        return instantiate_partitions2(
            policies, shape_in_partitions, partition_shape, std::move(instantiator));
    }
    else
    {
        // Instantiate each partition individually
        InstantiatePartition<Policies, rank<Shape>, Functor> instantiator{
            policies, shape_in_partitions, partition_shape, partition_creator, std::move(localities)};
        return instantiate_partitions2(
            policies, shape_in_partitions, partition_shape, std::move(instantiator));
    }
}

//     template<
//         typename Partitions>
//     class ValidateVisitor:
//         public ConstPartitionVisitor<Partitions>
//     {
//
//     private:
//
//         using Base = ConstPartitionVisitor<Partitions>;
//
//         using Partition = PartitionT<Partitions>;
//
//         using Offset = typename Partition::Offset;
//
//         using Shape = ShapeT<Partitions>;
//
//         using Cursor = typename Base::Cursor;
//
//     public:
//
//         ValidateVisitor(
//             Partitions const& partitions,
//             Shape const& shape):
//
//             Base{partitions},
//             _array_shape{shape}
//
//         {
//             _offset.fill(0);
//         }
//
//         void operator()()
//         {
//             // Given a cell, verify it is located within the array's shape, at
//             // the expected offset
//
//             Partition const& partition{this->partition()};
//             partition.wait();
//             Offset const partition_offset{partition.offset().get()};
//             _partition_shape = partition.shape().get();
//             Cursor const& cursor{this->cursor()};
//
//             if(partition_offset != _offset) {
//                 throw std::runtime_error(fmt::format(
//                     "Partition {} is not located at the expected offset {} "
//                     "in the array but at {}",
//                     cursor.current_cell(), _offset, partition_offset));
//             }
//
//             auto const dimension_idx{cursor.dimension_idx()};
//
//             // Given the current dimension, the next partition should be
//             // offset by the offset of the current partition plus its shape
//             _offset[dimension_idx] += _partition_shape[dimension_idx];
//         }
//
//         void leaving_current_dimension()
//         {
//             // dimension_idx is still pointing to the current (inner) dimension
//             // In case of a raster, cells are visited row-wise.
//
//             // When leaving a dimension, the offset for the inner dimension
//             // + the extent for that dimension of the last seen partition must
//             // match the partitioned array extent. Since we just visited
//             // the last cell in the current dimension, the offset is already
//             // located at the one-past-the-last cell. This value must equal
//             // the extent of the array's shape in the current dimension.
//
//             Cursor const& cursor{this->cursor()};
//             auto const dimension_idx{cursor.dimension_idx()};
//
//             if(_offset[dimension_idx] != _array_shape[dimension_idx])
//             {
//                 throw std::runtime_error(fmt::format(
//                     "Partition {} with shape {} does not fit within the array "
//                     "shape {}",
//                     cursor.current_cell(), _partition_shape, _array_shape));
//             }
//
//             // Prepare for leaving the current dimension. In case of a raster,
//             // we must increase the row offset by the number of rows in the
//             // last seen partition and set the column offset to zero (similar
//             // to a carriage return). We will be iterating over the cells
//             // in next row next.
//             lue_assert(dimension_idx > 0);
//             _offset[dimension_idx - 1] += _partition_shape[dimension_idx - 1];
//             _offset[dimension_idx] = 0;
//         }
//
//     private:
//
//         //! Shape of the array we are validating
//         Shape const    _array_shape;
//
//         //! Offset during visit
//         Offset         _offset;
//
//         //! Shape of last visited partition
//         Shape          _partition_shape;
//
//     };
//
//
//     template<
//         typename Element,
//         Rank rank>
//     void assert_invariants()
//     {
//
//         lue_assert(_partitions.shape() == _localities.shape());
//
//         // The array is either empty, or all localities are valid / known
//         lue_assert(
//             _localities.empty() ||
//             std::all_of(_localities.begin(), _localities.end(),
//                 [](hpx::id_type const locality_id)
//                 {
//                     return bool{locality_id};
//                 }));
//
//         // WARNING This is an expensive and synchronous operation. Never do
//         //     this in production code.
//
//     #ifndef NDEBUG
//
//         // Visit all partitions and see whether they fit exactly within
//         // the shape of the array
//
//         Shape begin_indices;
//         begin_indices.fill(0);
//         Shape const end_indices{_partitions.shape()};
//
//         ValidateVisitor validate{_partitions, _shape};
//
//         visit_array(begin_indices, end_indices, validate);
//
//     #endif
//     }

}  // namespace detail


template<
    typename Element,
    lue::Rank rank>
class InstantiateUninitialized
{

    public:

        using OutputElement = Element;
        using Partition = lue::ArrayPartition<OutputElement, rank>;
        using Offset = lue::OffsetT<Partition>;
        using Shape = lue::ShapeT<Partition>;

        static constexpr bool instantiate_per_locality{false};

        Partition instantiate(
            hpx::id_type const locality_id,
            lue::Index const /* partition_idx */,
            Offset const& offset,
            Shape const& partition_shape)
        {
            return hpx::async(

                    [locality_id, offset, partition_shape]()
                    {
                        return Partition{locality_id, offset, partition_shape};
                    }

                );
        }

    private:

};


template<
    typename Element,
    lue::Rank rank>
class InstantiateFilled
{

    public:

        using OutputElement = Element;
        using Partition = lue::ArrayPartition<OutputElement, rank>;
        using Offset = lue::OffsetT<Partition>;
        using Shape = lue::ShapeT<Partition>;

        static constexpr bool instantiate_per_locality{false};

        InstantiateFilled(
            Element const fill_value):

            _fill_value{fill_value}

        {
        }

        Partition instantiate(
            hpx::id_type const locality_id,
            lue::Index const /* partition_idx */,
            Offset const& offset,
            Shape const& partition_shape)
        {
            return hpx::async(

                    [locality_id, offset, partition_shape, fill_value=_fill_value]()
                    {
                        return Partition{locality_id, offset, partition_shape, fill_value};
                    }

                );
        }

    private:

        Element const _fill_value;

};


namespace policy::create_partitioned_array {

    template<
        typename OutputElement>
    using DefaultPolicies = policy::DefaultPolicies<
        OutputElements<OutputElement>,
        InputElements<>>;

}  // namespace policy::create_partitioned_array


/*!
    @brief      Create a partitioned array
    @param      policies Operation policies
    @param      array_shape Shape of array to create
    @param      partition_shape Shape of partitions to create
    @param      partition_creator Functor responsible for instantiating the partitions
    @param      clamp_mode How to handle the partitions at the sides to
                make an array of @a array_shape
    @return     Newly created partitioned array

    This function creates a new array pointing to partitions that are
    distributed over the available localities. It is responsible for
    creating tasks that will execute on those localities. These tasks
    use the functor passed in to instantitiate the partitions.

    Possible uses of this function include creating an array and:
    - Fill all elements with some value.
    - Fill all elements with random values.
    - Fill all elements with values read from a file on disk.
*/
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
    using OutputArray = PartitionedArray<Element, rank<Shape>>;

    // Given the shape of the array and the shape of the array partitions,
    // determine the shape of the array in partitions

    Shape const shape_in_partitions = lue::shape_in_partitions(array_shape, partition_shape);

    // OutputArray array{detail::instantiate_partitions(
    auto [localities, partitions] = detail::instantiate_partitions(
        policies, shape_in_partitions, partition_shape, partition_creator);

    // In most cases, the array just created is too large. Some cells
    // extent beyond the array size requested. How to handle this depends
    // on the clamp_mode passed in.

    switch(clamp_mode)
    {
        case OutputArray::ClampMode::shrink:
        {
            // Fix too large extent by shrinking overflowing partitions. This
            // may result is partitions that are too small for the focal
            // algorithms. Therefore, the alternative approach is more robust.
            detail::clamp_array_shrink<Element, rank<Shape>>(
                partitions, array_shape, shape_in_partitions, partition_shape);
            break;
        }
        case OutputArray::ClampMode::merge:
        {
            // Fix too large extent by merging relevant part of overflowing
            // partitions with bordering partitions. These will become larger
            // than the requested partition size. But focal algorithms will
            // behave more robust. Partitions in the array will be at least as
            // large as the partitions shape requested.
            detail::clamp_array_merge<Element, rank<Shape>>(
                localities, partitions, array_shape, shape_in_partitions, partition_shape);
            break;
        }
    }

    // TODO
    // assert_invariants();

    return OutputArray{array_shape, localities, std::move(partitions)};
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
