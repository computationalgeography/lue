#pragma once
#include "hpx/runtime_distributed/find_localities.hpp"
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/core/domain_decomposition.hpp"
#include "lue/framework/core/hilbert_curve.hpp"
#include "lue/framework/core/index_util.hpp"
#include "lue/framework/core/linear_curve.hpp"
#include "lue/framework/core/math.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include "lue/framework/serial_route.hpp"
#include <optional>


namespace lue {
    namespace detail {

        class LocalityIdxByPartitionIdx
        {

            public:

                LocalityIdxByPartitionIdx(Index const nr_partitions, Index const nr_localities):

                    _nr_partitions{nr_partitions},
                    _nr_localities{nr_localities}

                {
                    lue_hpx_assert(_nr_partitions > 0);
                    lue_hpx_assert(_nr_localities > 0);
                }


                Index operator()(Index const partition_idx) const
                {
                    return map_to_range(
                        Index{0}, _nr_partitions - 1, Index{0}, _nr_localities - 1, partition_idx);
                }


            private:

                Index _nr_partitions;

                Index _nr_localities;
        };


        template<typename Policies, Rank rank, typename Functor>
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
                    Shape const& array_shape,
                    Shapes&& partition_shapes):

                    _policies{policies},
                    _localities{partition_shapes.shape()},
                    _array_shape{array_shape},
                    _partitions{partition_shapes.shape()},
                    _partition_shape{partition_shapes[0]},
                    _partition_creator{partition_creator},
                    _locality_idx_by_partition_idx{nr_elements(_partitions), Index(localities.size())},
                    _localities_vector{std::forward<std::vector<hpx::id_type>>(localities)},
                    _partition_shapes{std::move(partition_shapes)}

                {
                    lue_hpx_assert(
                        std::none_of(
                            _localities.begin(),
                            _localities.end(),
                            [](hpx::id_type const locality_id) { return bool{locality_id}; }));
                    lue_hpx_assert(
                        std::all_of(
                            _localities_vector.begin(),
                            _localities_vector.end(),
                            [](hpx::id_type const locality_id) { return bool{locality_id}; }));
                }


                Localities localities()
                {
                    // Sink return. Call this method only once...
                    lue_hpx_assert(!_localities.empty());
                    lue_hpx_assert(
                        std::all_of(
                            _localities.begin(),
                            _localities.end(),
                            [](hpx::id_type const locality_id) { return bool{locality_id}; }));
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

                    lue_hpx_assert(
                        std::all_of(
                            _localities.begin(),
                            _localities.end(),
                            [](hpx::id_type const locality_id) { return bool{locality_id}; }));
                }
#endif


            protected:

                hpx::id_type locality_id(Index const partition_idx)  // Along curve!
                {
                    // partition_idx is an index along a curve visiting all
                    // partitions
                    return _localities_vector[_locality_idx_by_partition_idx(partition_idx)];
                }


                template<typename... Idxs>
                Offset offset(Idxs const... partition_idxs)  // In array!
                {
                    Offset offset{partition_idxs...};

                    for (std::size_t d = 0; d < rank; ++d)
                    {
                        offset[d] *= _partition_shape[d];
                    }

                    return offset;
                }


                template<typename... Idxs>
                Shape const& partition_shape(Idxs const... partition_idxs)  // In array!
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

                Shape _array_shape;

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


        template<typename Policies, Rank rank, typename Functor>
        class InstantiatePartition: public InstantiatePartitionsBase<Policies, rank, Functor>
        {

            public:

                using Base = InstantiatePartitionsBase<Policies, rank, Functor>;


                InstantiatePartition(
                    Policies const& policies,
                    Functor const& partition_creator,
                    std::vector<hpx::id_type>&& localities,
                    typename Base::Shape const& array_shape,
                    typename Base::Shapes&& partition_shapes):

                    Base{
                        policies,
                        partition_creator,
                        std::forward<std::vector<hpx::id_type>>(localities),
                        array_shape,
                        std::move(partition_shapes)}

                {
                }


                template<typename... Idxs>
                void operator()(
                    Index const partition_idx_along_curve,  // Along curve!
                    Idxs const... partition_idxs)           // In array!
                {
                    Index const partition_idx = idxs_to_linear_idx(
                        Indices<Index, rank>{partition_idxs...}, this->_partitions.shape());
                    hpx::id_type const locality_id{this->locality_id(partition_idx_along_curve)};
                    this->_localities(partition_idxs...) = locality_id;
                    this->_partitions(partition_idxs...) = this->_partition_creator.instantiate(
                        locality_id,
                        this->_policies,
                        this->_array_shape,
                        this->_partitions.shape(),
                        partition_idx_along_curve,  // Along curve
                        partition_idx,              // Linear
                        this->offset(partition_idxs...),
                        this->partition_shape(partition_idxs...));
                }


            private:
        };


        template<typename Policies, Rank rank, typename Functor>
        class InstantiatePartitions: public InstantiatePartitionsBase<Policies, rank, Functor>
        {

            public:

                using Base = InstantiatePartitionsBase<Policies, rank, Functor>;


                InstantiatePartitions(
                    Policies const& policies,
                    Functor const& partition_creator,
                    std::vector<hpx::id_type>&& localities,
                    typename Base::Shape const& array_shape,
                    typename Base::Shapes&& partition_shapes):

                    Base{
                        policies,
                        partition_creator,
                        std::forward<std::vector<hpx::id_type>>(localities),
                        array_shape,
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


                template<typename... Idxs>
                void operator()(
                    Index const partition_idx,     // Along curve!
                    Idxs const... partition_idxs)  // In array!
                {
                    hpx::id_type const locality_id{this->locality_id(partition_idx)};

                    if (partition_idx == 0)
                    {
                        prepare_for_new_locality(locality_id);
                    }

                    typename Base::Offset const offset{this->offset(partition_idxs...)};
                    typename Base::Shape const partition_shape{this->partition_shape(partition_idxs...)};

                    bool const same_locality{locality_id == _current_locality_id};
                    bool const last_partition{partition_idx == Index(this->nr_partitions()) - 1};

                    if (same_locality)
                    {
                        cache_partition_information(offset, partition_shape, partition_idxs...);
                    }
                    else
                    {
                        instantiate_partitions<Idxs...>();
                        prepare_for_new_locality(locality_id);
                        cache_partition_information(offset, partition_shape, partition_idxs...);
                    }

                    if (last_partition)
                    {
                        instantiate_partitions<Idxs...>();
                    }
                }


            private:

                using Offsets = std::vector<typename Base::Offset>;
                using Idxs = std::vector<std::array<Index, rank>>;


                void prepare_for_new_locality(hpx::id_type const locality_id)
                {
                    lue_hpx_assert(locality_id);
                    _current_locality_id = locality_id;
                    _offsets.clear();
                    _partition_shapes.clear();
                    _idxs.clear();
                }


                template<typename... Idxs>
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


                template<typename... Idxs>
                void instantiate_partitions()
                {
                    // Ask for the creation of a set of partitions and assign
                    // the resulting partition clients to the correct location
                    // in the array.
                    using Partitions = typename Base::Partitions;
                    using Partition = PartitionT<Partitions>;

                    hpx::future<std::vector<Partition>> partitions_f{this->_partition_creator.instantiate(
                        _current_locality_id,
                        this->_policies,
                        this->_array_shape,
                        _offsets,
                        _partition_shapes)};

                    // The collection of partitions is ready. This does not
                    // imply that the partitions themselves are ready. This
                    // does not matter though.

                    // Assign the partitions to their correct location in the array
                    {
                        std::vector<Partition> partitions{partitions_f.get()};
                        lue_hpx_assert(partitions.size() == _idxs.size());

                        for (std::size_t i = 0; i < partitions.size(); ++i)
                        {
                            std::apply(

                                [this,
                                 locality_id = _current_locality_id,
                                 partition = std::move(partitions[i])](Idxs... idxs)
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


        template<typename Policies, typename Shape, typename Instantiator>
        std::tuple<
            Localities<rank<Shape>>,
            PartitionsT<PartitionedArray<OutputElementT<Instantiator>, rank<Shape>>>>
        instantiate_partitions2(
            Policies const& /* policies */,
            Shape const& shape_in_partitions,
            Shape const& /* partition_shape */,
            Instantiator&& instantiator)  // The instantiator is ours
        {
            constexpr Rank rank{lue::rank<Shape>};

            // Pass a reference to the instantiator to the curve functions. We
            // need access to the results of the visit.

            if constexpr (rank == 2)  //  || rank == 3)
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


        template<typename Policies, typename Shape, typename Functor>
        auto instantiate_partitions(
            Policies const& policies,
            Shape const& array_shape,
            Shape const& partition_shape,
            Functor const& partition_creator)
            -> std::tuple<
                Localities<rank<Shape>>,
                PartitionsT<PartitionedArray<OutputElementT<Functor>, rank<Shape>>>>
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

            if (!(BuildOptions::build_qa && BuildOptions::qa_with_tests))
            {
                // In general, the number of localities should be smaller than
                // the number of partitions. Otherwise more hardware is used than
                // necessary. The exception is when we are building with tests
                // turned on. Tests may have to run on more localities than there
                // are partitions.
                if (nr_partitions < nr_localities)
                {
                    throw std::runtime_error(
                        std::format(
                            "Not enough partitions to use all localities ({} < {})",
                            nr_partitions,
                            nr_localities));
                }
            }

            lue_hpx_assert(hpx::find_here() == hpx::find_root_locality());
            lue_hpx_assert(localities[0] == hpx::find_root_locality());

            // for(auto const locality: localities)
            // {
            //     std::cout << locality << ", ";
            // }
            // std::cout << std::endl;

            // auto const& partitioner{hpx::resource::get_partitioner()};
            // lue_hpx_assert(partitioner.is_valid());

            // std::cout << "numa domains:" << std::endl;
            // for(auto const& numa_domain: partitioner.numa_domains())
            // {
            //     std::cout << "  " << numa_domain.id() << std::endl;
            // }

            // auto const& topology{hpx::threads::create_topology()};
            // topology.print_hwloc(std::cout);

            if constexpr (Functor::instantiate_per_locality)
            {
                // Per locality, instantiate all partitions in one go
                InstantiatePartitions<Policies, rank<Shape>, Functor> instantiator{
                    policies,
                    partition_creator,
                    std::move(localities),
                    array_shape,
                    std::move(partition_shapes)};
                return instantiate_partitions2(
                    policies, shape_in_partitions, partition_shape, std::move(instantiator));
            }
            else
            {
                // Instantiate each partition individually
                InstantiatePartition<Policies, rank<Shape>, Functor> instantiator{
                    policies,
                    partition_creator,
                    std::move(localities),
                    array_shape,
                    std::move(partition_shapes)};
                return instantiate_partitions2(
                    policies, shape_in_partitions, partition_shape, std::move(instantiator));
            }
        }


        template<typename Policies, typename InputPartitions, typename Functor>
        auto instantiate_partitions(
            Policies const& policies,
            ShapeT<InputPartitions> const& array_shape,
            InputPartitions const& input_partitions,
            Functor const& partition_creator)
            -> std::tuple<
                Localities<rank<InputPartitions>>,
                PartitionsT<PartitionedArray<OutputElementT<Functor>, rank<InputPartitions>>>>
        {
            // Create partitions. Each of these partitions will be a component client instance
            // referring to a, possibly remote, component server instance.

            Rank const rank{lue::rank<InputPartitions>};

            using Offset = OffsetT<InputPartitions>;
            using Shape = ShapeT<InputPartitions>;

            using OutputElement = OutputElementT<Functor>;
            using OutputArray = PartitionedArray<OutputElement, rank>;
            using OutputPartitions = PartitionsT<OutputArray>;
            using OutputPartition = PartitionT<OutputPartitions>;

            Shape const shape_in_partitions{input_partitions.shape()};

            OutputPartitions output_partitions{shape_in_partitions};
            Count const nr_partitions = lue::nr_elements(shape_in_partitions);
            lue_hpx_assert(output_partitions.nr_elements() == nr_partitions);

            Localities<rank> localities{shape_in_partitions};

            lue_hpx_assert(hpx::find_here() == hpx::find_root_locality());

            for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
            {
                // For each partition:
                // - Determine the locality it is located on
                // - Spawn a task on that locality, passing in the input partition and returning
                //   an output partition

                lue_hpx_assert(input_partitions[partition_idx].valid());

                input_partitions[partition_idx].get();

                output_partitions[partition_idx] = hpx::dataflow(
                    hpx::launch::async,
                    hpx::unwrapping(

                        [policies,
                         array_shape,
                         shape_in_partitions,
                         partition_idx = partition_idx,
                         partition_creator](
                            hpx::id_type const locality_id, Offset const& offset, Shape const& shape)
                            -> OutputPartition
                        {
                            return partition_creator.instantiate(
                                locality_id,
                                policies,
                                array_shape,
                                shape_in_partitions,
                                partition_idx,
                                offset,
                                shape);
                        }

                        ),
                    hpx::get_colocation_id(input_partitions[partition_idx].get_id()),
                    input_partitions[partition_idx].offset(),
                    input_partitions[partition_idx].shape());
            }

            return {std::move(localities), std::move(output_partitions)};
        }

    }  // namespace detail


    template<typename Element, lue::Rank rank>
    class InstantiateDefaultInitialized
    {

        public:

            using OutputElement = Element;
            using Partition = lue::ArrayPartition<OutputElement, rank>;
            using Offset = lue::OffsetT<Partition>;
            using Shape = lue::ShapeT<Partition>;


            static constexpr bool instantiate_per_locality{false};


            template<typename Policies>
            auto instantiate(
                hpx::id_type const locality_id,
                [[maybe_unused]] Policies const& policies,
                [[maybe_unused]] Shape const& array_shape,
                [[maybe_unused]] Shape const& shape_in_partitions,
                [[maybe_unused]] lue::Index const partition_idx_along_curve,
                [[maybe_unused]] lue::Index const partition_idx,
                Offset const& offset,
                Shape const& partition_shape) -> Partition
            {
                return hpx::async(

                    [locality_id, offset, partition_shape]()
                    { return Partition{locality_id, offset, partition_shape}; }

                );
            }


        private:
    };


    template<typename Element, lue::Rank rank>
    class InstantiateFilled
    {

        public:

            using OutputElement = Element;
            using Partition = lue::ArrayPartition<OutputElement, rank>;
            using Offset = lue::OffsetT<Partition>;
            using Shape = lue::ShapeT<Partition>;

            static constexpr bool instantiate_per_locality{false};


            InstantiateFilled(hpx::shared_future<Element> const& fill_value):

                _fill_value{fill_value}

            {
            }


            template<typename Policies>
            auto instantiate(
                hpx::id_type const locality_id,
                [[maybe_unused]] Policies const& policies,
                [[maybe_unused]] Shape const& array_shape,
                [[maybe_unused]] Shape const& shape_in_partitions,
                [[maybe_unused]] lue::Index const partition_idx_along_curve,
                [[maybe_unused]] lue::Index const partition_idx,
                Offset const& offset,
                Shape const& partition_shape) const -> Partition
            {
                return _fill_value.then(
                    [locality_id, offset, partition_shape](hpx::shared_future<Element> const& fill_value)
                    { return Partition{locality_id, offset, partition_shape, fill_value.get()}; });
            }


        private:

            hpx::shared_future<Element> _fill_value;
    };


    /*!
        @brief      Functor to use when creating a partitioned array given an existing buffer
                    containing values to copy into the array partitions
        @tparam     BufferHandle Type of handle of the buffer. Instances must manage a reference
                    count for the buffer. This count must be increased upon copying, and
                    decreased upon destruction.
        @tparam     Rank Rank of array to return
    */
    template<typename BufferHandle, Rank rank>
    class InstantiateFromBuffer
    {

        public:

            using Element = ElementT<BufferHandle>;

            using OutputElement = Element;
            using Partition = lue::ArrayPartition<OutputElement, rank>;
            using Offset = lue::OffsetT<Partition>;
            using Shape = lue::ShapeT<Partition>;

            static constexpr bool instantiate_per_locality{false};

            // Type of function that returns a pointer to the buffer
            using GrabBuffer = std::function<Element*(BufferHandle)>;

            InstantiateFromBuffer(BufferHandle buffer_handle, GrabBuffer grab_buffer):

                _buffer_handle{std::move(buffer_handle)},
                _grab_buffer{std::move(grab_buffer)},
                _no_data_value{}

            {
                assert(!_no_data_value);
            }


            InstantiateFromBuffer(
                BufferHandle buffer_handle, GrabBuffer grab_buffer, Element const& no_data_value):

                _buffer_handle{std::move(buffer_handle)},
                _grab_buffer{std::move(grab_buffer)},
                _no_data_value{std::make_optional<Element>(no_data_value)}

            {
                assert(_no_data_value);
            }


            InstantiateFromBuffer(
                BufferHandle buffer_handle,
                GrabBuffer grab_buffer,
                std::optional<Element> const& no_data_value):

                _buffer_handle{std::move(buffer_handle)},
                _grab_buffer{std::move(grab_buffer)},
                _no_data_value{no_data_value}

            {
            }


            template<typename Policies>
            auto instantiate(
                hpx::id_type const locality_id,
                Policies const& policies,
                Shape const& array_shape,
                [[maybe_unused]] Shape const& shape_in_partitions,
                [[maybe_unused]] lue::Index const partition_idx_along_curve,
                [[maybe_unused]] lue::Index const partition_idx,
                Offset const& offset,
                Shape const& partition_shape) -> Partition
            {
                using Data = lue::ArrayPartitionData<OutputElement, rank>;

                auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                // A single instance of this class is used to instantiate all new partitions. This happens
                // asynchronously. A partition client (a future to the server instance) is returned
                // immediately, but the server instance itself is not created yet. To be able to create the
                // server instance later on, when the InstantiateFromBuffer instance is already gone, we still
                // need to be able to access to the buffer. Because of that, we pass a copy of the
                // BufferHandle instance and the GrabBuffer function to the lambda. Copying the BufferHandle
                // instance increases the reference count and keeps the underlying buffer alive.

                return hpx::async(

                    [locality_id,
                     ondp,
                     array_shape,
                     offset,
                     partition_shape,
                     buffer_handle = _buffer_handle,  // Copy, increases reference count
                     grab_buffer = _grab_buffer,
                     no_data_value = _no_data_value]()
                    {
                        // Create a partition instance and copy the relevant cells from the input buffer to
                        // the partition instance

                        // A 1D array of elements to copy
                        Element const* buffer = grab_buffer(buffer_handle);
                        static_assert(rank == 2);  // For now

                        Data data{partition_shape};
                        Element const* source = buffer + (offset[0] * array_shape[1]) + offset[1];

                        // Iterate over all rows in the partition
                        for (Index idx0 = 0; idx0 < partition_shape[0]; ++idx0)
                        {
                            Element* destination = &data(idx0, 0);

                            // Copy a single row from the buffer into the partition
                            std::copy_n(source, partition_shape[1], destination);

                            if (no_data_value)
                            {
                                // Mark occurrences of no_data_value in the input as no-data
                                for (Index i = 0; i < partition_shape[1]; ++i)
                                {
                                    if (destination[i] == *no_data_value)
                                    {
                                        ondp.mark_no_data(destination, i);
                                    }
                                }
                            }

                            source += array_shape[1];
                        }

                        return Partition{locality_id, offset, std::move(data)};
                    }

                );
            }


        private:

            //! Reference counted handle to underlying buffer
            BufferHandle _buffer_handle;

            //! Functor providing access to the buffer
            GrabBuffer _grab_buffer;

            //! Value in buffer having no-data semantics
            std::optional<Element> _no_data_value;
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

        template<typename OutputElement>
        using DefaultPolicies =
            policy::DefaultPolicies<AllValuesWithinDomain<>, OutputElements<OutputElement>, InputElements<>>;

        template<typename OutputElement>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<AllValuesWithinDomain<>, OutputElements<OutputElement>, InputElements<>>;

    }  // namespace policy::create_partitioned_array


    template<typename Functor>
    concept PartitionInstantiator = requires(Functor functor) { functor.instantiate_per_locality; };


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
    template<typename Policies, Rank rank, PartitionInstantiator Functor>
    auto create_partitioned_array(
        Policies const& policies,
        Shape<Count, rank> const& array_shape,
        Shape<Count, rank> const& partition_shape,
        Functor const& partition_creator) -> PartitionedArray<OutputElementT<Functor>, rank>
    {
        // Create the array partitions that, together make up the partitioned
        // array.

        using Element = OutputElementT<Functor>;
        using OutputArray = PartitionedArray<Element, rank>;

        // Given the shape of the array and the shape of the array partitions,
        // determine the shape of the array in partitions

        auto [localities, partitions] =
            detail::instantiate_partitions(policies, array_shape, partition_shape, partition_creator);

        return OutputArray{array_shape, std::move(localities), std::move(partitions)};
    }


    template<typename Policies, typename RouteID, Rank rank, PartitionInstantiator Functor>
    auto create_partitioned_array(
        Policies const& policies, SerialRoute<RouteID, rank> const& route, Functor const& partition_creator)
        -> PartitionedArray<OutputElementT<Functor>, rank>
    {
        // Use the route passed in as a clone / template for creating the new array. The shape
        // of the output array and the individual partitions, and the distribution of the
        // partitions over the localities must be the same as that / those of the input array.

        using Element = OutputElementT<Functor>;
        using OutputArray = PartitionedArray<Element, rank>;

        auto [localities, partitions] =
            detail::instantiate_partitions(policies, route.shape(), route.partitions(), partition_creator);

        return OutputArray{route.shape(), std::move(localities), std::move(partitions)};
    }


    /*!
        @overload

        The partition shape is based on @a default_partition_shape.
    */
    template<typename Policies, Rank rank, PartitionInstantiator Functor>
    auto create_partitioned_array(
        Policies const& policies, Shape<Count, rank> const& array_shape, Functor const& partition_creator)
        -> PartitionedArray<OutputElementT<Functor>, rank>
    {
        return create_partitioned_array<Policies, rank, Functor>(
            policies, array_shape, default_partition_shape(array_shape), partition_creator);
    }


    template<Rank rank, PartitionInstantiator Functor>
    auto create_partitioned_array(
        Shape<Count, rank> const& array_shape,
        Shape<Count, rank> const& partition_shape,
        Functor const& partition_creator) -> PartitionedArray<OutputElementT<Functor>, rank>
    {
        using Element = OutputElementT<Functor>;
        using Policies = policy::create_partitioned_array::DefaultPolicies<Element>;

        return create_partitioned_array<Policies, rank, Functor>(
            Policies{}, array_shape, partition_shape, partition_creator);
    }


    template<Rank rank, PartitionInstantiator Functor>
    auto create_partitioned_array(Shape<Count, rank> const& array_shape, Functor const& partition_creator)
        -> PartitionedArray<OutputElementT<Functor>, rank>
    {
        return create_partitioned_array<rank, Functor>(
            array_shape, default_partition_shape(array_shape), partition_creator);
    }


    template<typename RouteID, Rank rank, PartitionInstantiator Functor>
    auto create_partitioned_array(SerialRoute<RouteID, rank> const& route, Functor const& partition_creator)
        -> PartitionedArray<OutputElementT<Functor>, rank>
    {
        using Policies = policy::create_partitioned_array::DefaultPolicies<OutputElementT<Functor>>;

        return create_partitioned_array<Policies, rank, Functor>(Policies{}, route, partition_creator);
    }


    // create array, fill with default initialized value -----------------------------------------------------

    template<typename Element, Rank rank, std::enable_if_t<std::is_arithmetic_v<Element>>* = nullptr>
    auto create_partitioned_array(
        Shape<Count, rank> const& array_shape, Shape<Count, rank> const& partition_shape)
        -> PartitionedArray<Element, rank>
    {
        using Policies = policy::create_partitioned_array::DefaultPolicies<Element>;
        using Functor = InstantiateDefaultInitialized<Element, rank>;

        return create_partitioned_array<Policies, rank, Functor>(
            Policies{}, array_shape, partition_shape, Functor{});
    }


    template<typename Element, Rank rank, std::enable_if_t<std::is_arithmetic_v<Element>>* = nullptr>
    auto create_partitioned_array(Shape<Count, rank> const& array_shape) -> PartitionedArray<Element, rank>
    {
        return create_partitioned_array<Element, rank>(array_shape, default_partition_shape(array_shape));
    }


    // create array, fill with fill value --------------------------------------------------------------------

    template<typename Element, Rank rank>
        requires std::is_arithmetic_v<Element>
    auto create_partitioned_array(
        Shape<Count, rank> const& array_shape,
        Shape<Count, rank> const& partition_shape,
        Scalar<Element> const& fill_value) -> PartitionedArray<Element, rank>
    {
        using Policies = policy::create_partitioned_array::DefaultPolicies<Element>;
        using Functor = InstantiateFilled<Element, rank>;

        return create_partitioned_array(
            Policies{}, array_shape, partition_shape, Functor{fill_value.future()});
    }


    template<typename Element, Rank rank>
        requires std::is_arithmetic_v<Element>
    auto create_partitioned_array(
        Shape<Count, rank> const& array_shape,
        Shape<Count, rank> const& partition_shape,
        Element const fill_value) -> PartitionedArray<Element, rank>
    {
        return create_partitioned_array(array_shape, partition_shape, Scalar<Element>{fill_value});
    }


    template<typename Element, Rank rank>
        requires std::is_arithmetic_v<Element>
    auto create_partitioned_array(Shape<Count, rank> const& array_shape, Scalar<Element> const& fill_value)
        -> PartitionedArray<Element, rank>
    {
        return create_partitioned_array(array_shape, default_partition_shape(array_shape), fill_value);
    }


    template<typename Element, Rank rank>
        requires std::is_arithmetic_v<Element>
    auto create_partitioned_array(Shape<Count, rank> const& array_shape, Element const fill_value)
        -> PartitionedArray<Element, rank>
    {
        return create_partitioned_array(
            array_shape, default_partition_shape(array_shape), Scalar<Element>{fill_value});
    }

}  // namespace lue
