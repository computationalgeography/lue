#pragma once
#include "lue/framework/core/array.hpp"
#include "lue/framework/partitioned_array/array_partition.hpp"


namespace lue {

    template<Rank rank>
    using Localities = Array<hpx::id_type, rank>;


    /*!
        @brief      Class template for representing partitioned arrays
        @tparam     Element Type for representing element values
        @tparam     rank Array rank

        The array is partitioned. Partitions can be located in multiple localities (processes).

        PartitionedArray is a move-only type.
    */
    template<typename Element, Rank rank>
    class PartitionedArray
    {

        private:

            using PartitionClient = ArrayPartition<Element, rank>;

        public:

            //! Type of client-side representation of an array partition
            using Partition = PartitionClient;

            //! Type of the collection of partitions
            using Partitions = ArrayPartitionData<Partition, rank>;

            using Count = typename Partitions::Count;

            using Offset = typename PartitionClient::Offset;

            using Shape = typename Partitions::Shape;


            /*!
                @brief      Default-construct an instance with an empty shape

                The array will have zero elements.
            */
            PartitionedArray():

                _shape{},
                _localities{},
                _partitions{}

            {
                // Shape is filled with indeterminate values! This may or may not
                // matter, depending on what happens next with this instance. To be sure,
                // explicitly set the shape to empty.
                _shape.fill(0);

                assert_invariants();
            }


            PartitionedArray(PartitionedArray const& other) = delete;

            PartitionedArray(PartitionedArray&& other) = default;


            /*!
                @brief      Construct an instance
                @param      shape Shape of the array
                @param      localities Localities where the partitions are located
                @param      partitions Collection of array partitions

                The shape of the partitions together must equal the shape passed in
            */
            PartitionedArray(Shape const& shape, Localities<rank>&& localities, Partitions&& partitions):

                _shape{shape},
                _localities{std::move(localities)},
                _partitions{std::move(partitions)}

            {
                assert_invariants();
            }


            ~PartitionedArray() = default;

            auto operator=(PartitionedArray const& other) -> PartitionedArray& = delete;

            auto operator=(PartitionedArray&& other) -> PartitionedArray& = default;


            /*!
                @brief      Return the number of elements
            */
            auto nr_elements() const -> Count
            {
                return lue::nr_elements(_shape);
            }


            /*!
                @brief      Return the shape
            */
            auto shape() const -> Shape const&
            {
                return _shape;
            }


            /*!
                @brief      Return the localities
            */
            auto localities() const -> Localities<rank> const&
            {
                return _localities;
            }


            /*!
                @brief      Return the number of partitions
            */
            auto nr_partitions() const -> Count
            {
                return _partitions.nr_elements();
            }


            /*!
                @brief      Return the partitions
            */
            auto partitions() -> Partitions&
            {
                return _partitions;
            }


            /*!
                @brief      Return the partitions
            */
            auto partitions() const -> Partitions const&
            {
                return _partitions;
            }

        private:

            void assert_invariants() const
            {
                lue_hpx_assert(_partitions.shape() == _localities.shape());

                // The array is either empty, or all localities are valid / known
                lue_hpx_assert(
                    _localities.empty() ||
                    std::all_of(
                        _localities.begin(),
                        _localities.end(),
                        [](hpx::id_type const& locality_id) { return bool{locality_id}; }));
            }


            //! Shape of the partitioned array
            Shape _shape;

            //! Localities the partitions are located in
            Localities<rank> _localities;

            //! Array of partitions
            Partitions _partitions;
    };


    namespace detail {

        // Specialization for PartitionedArray
        template<typename E, Rank r>
        class ArrayTraits<PartitionedArray<E, r>>
        {

            public:

                using Element = E;

                static constexpr Rank rank = r;

                static constexpr bool is_partitioned_array{true};

                using Offset = typename PartitionedArray<E, r>::Offset;

                using Shape = typename PartitionedArray<E, r>::Shape;

                template<typename E_, Rank r_>
                using Partition = typename PartitionedArray<E_, r_>::Partition;

                template<typename E_, Rank r_>
                using Partitions = typename PartitionedArray<E_, r_>::Partitions;

                template<typename E_, Rank r_>
                using PartitionedArray = PartitionedArray<E_, r_>;

                template<typename E_, Rank r_>
                using Data = typename Partition<E_, r_>::Data;

                template<typename E_, Rank r_>
                using Component = typename PartitionedArray<E_, r_>::Partition;

                template<typename E_, Rank r_>
                using Components = typename PartitionedArray<E_, r_>::Partitions;
        };


        // Specialization for PartitionedArray::Partitions
        template<typename E, Rank r>
        class ArrayTraits<ArrayPartitionData<ArrayPartition<E, r>, r>>
        {

            public:

                using Element = E;

                constexpr static Rank rank = r;

                using Shape = typename ArrayPartitionData<ArrayPartition<E, r>, r>::Shape;

                template<typename E_, Rank r_>
                using Partition = ArrayPartition<E_, r_>;

                template<typename E_, Rank r_>
                using Partitions = ArrayPartitionData<ArrayPartition<E_, r_>, r_>;

                template<typename E_, Rank r_>
                using Component = ArrayPartition<E_, r_>;
        };

    }  // namespace detail


    /*!
        @brief      Return the shape of @a array
    */
    template<typename Element, Rank rank>
    static auto shape(PartitionedArray<Element, rank> const& array) -> ShapeT<PartitionedArray<Element, rank>>
    {
        return array.shape();
    }


    /*!
        @brief      Return the shape in partitions of @a array
    */
    template<typename Element, Rank rank>
    static auto shape_in_partitions(PartitionedArray<Element, rank> const& array)
        -> ShapeT<PartitionedArray<Element, rank>>
    {
        return array.partitions().shape();
    }


    /*!
        @brief      Return the number of partitions in @a array
    */
    template<typename Element, Rank rank>
    static auto nr_partitions(PartitionedArray<Element, rank> const& array) -> Count
    {
        return array.nr_partitions();
    }


    /*!
        @brief      Return the number of partitions in the collection of @a partitions
    */
    template<typename Element, Rank rank>
    static auto nr_partitions(ArrayPartitionData<ArrayPartition<Element, rank>, rank> const& partitions)
        -> Count
    {
        return partitions.nr_elements();
    }


    /*!
        @brief      Return the number of elements in @a array
    */
    template<typename Element, Rank rank>
    static auto nr_elements(PartitionedArray<Element, rank> const& array) ->
        typename PartitionedArray<Element, rank>::Count
    {
        return array.nr_elements();
    }

}  // namespace lue
