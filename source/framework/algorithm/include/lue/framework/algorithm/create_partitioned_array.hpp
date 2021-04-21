#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/core/domain_decomposition.hpp"
#include "lue/framework/core/hilbert_curve.hpp"
#include "lue/framework/core/linear_curve.hpp"
#include "lue/framework/core/math.hpp"


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
                    lue_hpx_assert(_nr_partitions > 0);
                    lue_hpx_assert(_nr_localities > 0);
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

                using Shapes = Array<Shape, rank>;


                InstantiatePartitionsBase(
                    Policies const& policies,
                    Functor const& partition_creator,
                    std::vector<hpx::id_type>&& localities,
                    Shapes&& partition_shapes):

                    _policies{policies},
                    _localities{partition_shapes.shape()},
                    _partitions{partition_shapes.shape()},
                    _partition_shape{partition_shapes[0]},
                    _partition_creator{partition_creator},
                    _locality_idx_by_partition_idx{nr_elements(_partitions), Index(localities.size())},
                    _localities_vector{std::forward<std::vector<hpx::id_type>>(localities)},
                    _partition_shapes{std::move(partition_shapes)}

                {
                    lue_hpx_assert(std::none_of(_localities.begin(), _localities.end(),
                        [](hpx::id_type const locality_id)
                        {
                            return bool{locality_id};
                        }));
                    lue_hpx_assert(std::all_of(_localities_vector.begin(), _localities_vector.end(),
                        [](hpx::id_type const locality_id)
                        {
                            return bool{locality_id};
                        }));
                }


                Localities localities()
                {
                    // Sink return. Call this method only once...
                    lue_hpx_assert(!_localities.empty());
                    lue_hpx_assert(std::all_of(_localities.begin(), _localities.end(),
                        [](hpx::id_type const locality_id)
                        {
                            return bool{locality_id};
                        }));
                    return std::move(_localities);
                }


                Partitions partitions()
                {
                    // Sink return. Call this method only once...
                    lue_hpx_assert(!_partitions.empty());
                    return std::move(_partitions);
                }


#ifndef NDEBUG
                void assert_invariants() const
                {
                    lue_hpx_assert(_partitions.shape() == _localities.shape());

                    lue_hpx_assert(std::all_of(_localities.begin(), _localities.end(),
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


                template<
                    typename... Idxs>
                Shape const& partition_shape(
                    Idxs const... partition_idxs)  // In array!
                {
                    return _partition_shapes(partition_idxs...);
                }


                std::size_t nr_partitions() const
                {
                    return nr_elements(_partitions);
                }


            protected:

                Policies _policies;

                Localities _localities;

                Partitions _partitions;

                //! Partition shape of (almost) all partitions
                Shape _partition_shape;

                Functor _partition_creator;


            private:

                LocalityIdxByPartitionIdx _locality_idx_by_partition_idx;

                std::vector<hpx::id_type> _localities_vector;

                //! Actual partition shapes
                Shapes _partition_shapes;

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
                    Functor const& partition_creator,
                    std::vector<hpx::id_type>&& localities,
                    typename Base::Shapes&& partition_shapes):

                    Base{policies, partition_creator,
                        std::forward<std::vector<hpx::id_type>>(localities),
                        std::move(partition_shapes)}

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
                            locality_id, this->_policies, partition_idx, this->offset(partition_idxs...),
                            this->partition_shape(partition_idxs...));
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
                    Functor const& partition_creator,
                    std::vector<hpx::id_type>&& localities,
                    typename Base::Shapes&& partition_shapes):

                    Base{policies, partition_creator,
                        std::forward<std::vector<hpx::id_type>>(localities),
                        std::move(partition_shapes)},
                    _current_locality_id{},
                    _offsets{},
                    _partition_shapes{},
                    _idxs{}

                {
                    lue_hpx_assert(!_current_locality_id);
                    lue_hpx_assert(_offsets.empty());
                    lue_hpx_assert(_partition_shapes.empty());
                    lue_hpx_assert(_idxs.empty());
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
                    typename Base::Shape const partition_shape{this->partition_shape(partition_idxs...)};

                    bool const same_locality{locality_id == _current_locality_id};
                    bool const last_partition{partition_idx == Index(this->nr_partitions()) - 1};

                    if(same_locality)
                    {
                        cache_partition_information(offset, partition_shape, partition_idxs...);
                    }
                    else
                    {
                        instantiate_partitions<Idxs...>();
                        prepare_for_new_locality(locality_id);
                        cache_partition_information(offset, partition_shape, partition_idxs...);

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
                    lue_hpx_assert(locality_id);
                    _current_locality_id = locality_id;
                    _offsets.clear();
                    _partition_shapes.clear();
                    _idxs.clear();
                }


                template<
                    typename... Idxs>
                void cache_partition_information(
                    typename Base::Offset const& offset,
                    typename Base::Shape const& partition_shape,
                    Idxs const... partition_idxs)
                {
                    // lue_hpx_assert(locality_id == _current_locality_id);
                    _offsets.push_back(offset);
                    _partition_shapes.push_back(partition_shape);
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
                            _current_locality_id, this->_policies, _offsets, _partition_shapes)};

                    // The collection of partitions is ready. This does not
                    // imply that the partitions themselves are ready. This
                    // does not matter though.

                    // Assign the partitions to their correct location in the array
                    {
                        std::vector<Partition> partitions{partitions_f.get()};
                        lue_hpx_assert(partitions.size() == _idxs.size());

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

                std::vector<typename Base::Shape> _partition_shapes;

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
                Shape const& array_shape,
                Shape const& partition_shape,
                Functor const& partition_creator)
        {
            // Create array containing partitions. Each of these partitions
            // will be a component client instance referring to a, possibly
            // remote, component server instance.

            using Element = OutputElementT<Functor>;
            using OutputArray = PartitionedArray<Element, rank<Shape>>;
            using Partitions = PartitionsT<OutputArray>;

            auto partition_shapes = lue::partition_shapes(array_shape, partition_shape);
            Shape const shape_in_partitions{partition_shapes.shape()};

            Partitions partitions{shape_in_partitions};
            Count const nr_partitions = lue::nr_elements(shape_in_partitions);
            lue_hpx_assert(partitions.nr_elements() == nr_partitions);

            std::vector<hpx::id_type> localities = hpx::find_all_localities();

            [[maybe_unused]] Count const nr_localities = localities.size();
            lue_hpx_assert(nr_localities > 0);

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

            lue_hpx_assert(hpx::find_here() == hpx::find_root_locality());
            lue_hpx_assert(localities[0] == hpx::find_root_locality());

            if constexpr (Functor::instantiate_per_locality)
            {
                // Per locality, instantiate all partitions in one go
                InstantiatePartitions<Policies, rank<Shape>, Functor> instantiator{
                    policies, partition_creator, std::move(localities), std::move(partition_shapes)};
                return instantiate_partitions2(
                    policies, shape_in_partitions, partition_shape, std::move(instantiator));
            }
            else
            {
                // Instantiate each partition individually
                InstantiatePartition<Policies, rank<Shape>, Functor> instantiator{
                    policies, partition_creator, std::move(localities), std::move(partition_shapes)};
                return instantiate_partitions2(
                    policies, shape_in_partitions, partition_shape, std::move(instantiator));
            }
        }

    }  // namespace detail


    template<
        typename Element,
        lue::Rank rank>
    class InstantiateDefaultInitialized
    {

        public:

            using OutputElement = Element;
            using Partition = lue::ArrayPartition<OutputElement, rank>;
            using Offset = lue::OffsetT<Partition>;
            using Shape = lue::ShapeT<Partition>;


            static constexpr bool instantiate_per_locality{false};


            template<
                typename Policies>
            Partition instantiate(
                hpx::id_type const locality_id,
                [[maybe_unused]] Policies const& policies,
                [[maybe_unused]] lue::Index const partition_idx,
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


            template<
                typename Policies>
            Partition instantiate(
                hpx::id_type const locality_id,
                [[maybe_unused]] Policies const& policies,
                [[maybe_unused]] lue::Index const partition_idx,
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


    template<
        typename Element,
        lue::Rank rank>
    class FunctorTraits<
        InstantiateFilled<Element, rank>>
    {

        public:

            static constexpr bool const is_functor{true};

    };


    template<
        typename Element,
        lue::Rank rank>
    class FunctorTraits<
        InstantiateDefaultInitialized<Element, rank>>
    {

        public:

            static constexpr bool const is_functor{true};

    };


    // TODO, as part of range algorithm
    // template<
    //     typename Element,
    //     lue::Rank rank>
    // class InstantiateRange
    // {
    // }


    // TODO, as part of uniform algorithm
    // template<
    //     typename Element,
    //     lue::Rank rank>
    // class InstantiateUniform
    // {
    // }


    // TODO, as part of unique algorithm
    // template<
    //     typename Element,
    //     lue::Rank rank>
    // class InstantiateUnique
    // {
    // }


    namespace policy::create_partitioned_array {

        template<
            typename OutputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<>,
            OutputElements<OutputElement>,
            InputElements<>>;

        template<
            typename OutputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<>,
            OutputElements<OutputElement>,
            InputElements<>>;

    }  // namespace policy::create_partitioned_array


    /*!
        @brief      Create a partitioned array
        @param      policies Operation policies
        @param      array_shape Shape of array to create
        @param      partition_shape Shape of partitions to create
        @param      partition_creator Functor responsible for instantiating the partitions
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
        typename Functor,
        typename Shape>
    PartitionedArray<OutputElementT<Functor>, rank<Shape>> create_partitioned_array(
        Policies const& policies,
        Shape const& array_shape,
        Shape const& partition_shape,
        Functor const& partition_creator)
    {
        // Create the array partitions that, together make up the partitioned
        // array. Note that the extent of this array might be too large,
        // given that we use a certain partition shape for all partitions.

        using Element = OutputElementT<Functor>;
        using OutputArray = PartitionedArray<Element, rank<Shape>>;

        // Given the shape of the array and the shape of the array partitions,
        // determine the shape of the array in partitions

        auto [localities, partitions] = detail::instantiate_partitions(
            policies, array_shape, partition_shape, partition_creator);

        return OutputArray{array_shape, localities, std::move(partitions)};
    }


    template<
        typename Functor,
        typename Shape,
        // Select cases where Functor is passed, instead of Element
        std::enable_if_t<is_functor_v<Functor>>* =nullptr>
    PartitionedArray<OutputElementT<Functor>, rank<Shape>> create_partitioned_array(
        Shape const& array_shape,
        Shape const& partition_shape,
        Functor const& partition_creator)
    {
        using Element = OutputElementT<Functor>;
        using Policies = policy::create_partitioned_array::DefaultPolicies<Element>;

        return create_partitioned_array(Policies{}, array_shape, partition_shape, partition_creator);
    }


    template<
        typename Element,
        typename Shape,
        // Select cases where Element is not a Functor (but it can be a class)
        std::enable_if_t<!is_functor_v<Element>>* =nullptr>
    PartitionedArray<Element, rank<Shape>> create_partitioned_array(
        Shape const& array_shape,
        Shape const& partition_shape)
    {
        using Policies = policy::create_partitioned_array::DefaultPolicies<Element>;
        using Functor = InstantiateDefaultInitialized<Element, rank<Shape>>;

        return create_partitioned_array(Policies{}, array_shape, partition_shape, Functor{});
    }


    template<
        typename Element,
        typename Shape,
        // Select cases where Element is not a Functor (but it can be a class)
        std::enable_if_t<!is_functor_v<Element>>* =nullptr>
    PartitionedArray<Element, rank<Shape>> create_partitioned_array(
        Shape const& array_shape,
        Shape const& partition_shape,
        Element const fill_value)
    {
        using Policies = policy::create_partitioned_array::DefaultPolicies<Element>;
        using Functor = InstantiateFilled<Element, rank<Shape>>;

        return create_partitioned_array(Policies{}, array_shape, partition_shape, Functor{fill_value});
    }

}  // namespace lue
