#pragma once
#include "lue/framework/core/array.hpp"
#include "lue/framework/partitioned_array/array_partition_decl.hpp"
#include "lue/framework/partitioned_array/export.hpp"


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
    class LUE_FPA_EXPORT PartitionedArray
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


            PartitionedArray();

            PartitionedArray(PartitionedArray const& other) = delete;

            PartitionedArray(PartitionedArray&& other) = default;


            PartitionedArray(Shape const& shape, Localities<rank>&& localities, Partitions&& partitions);

            ~PartitionedArray() = default;

            auto operator=(PartitionedArray const& other) -> PartitionedArray& = delete;

            auto operator=(PartitionedArray&& other) -> PartitionedArray& = default;


            auto nr_elements() const -> Count;

            auto shape() const -> Shape const&;

            auto localities() const -> Localities<rank> const&;

            auto nr_partitions() const -> Count;

            auto partitions() -> Partitions&;

            auto partitions() const -> Partitions const&;

        private:

            void assert_invariants() const;

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
