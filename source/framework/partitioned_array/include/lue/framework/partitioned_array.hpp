#pragma once
#include "lue/framework/core/array.hpp"
#include "lue/framework/core/debug.hpp"  // describe
#include "lue/framework/partitioned_array/array_partition.hpp"


namespace lue {

    template<Rank rank>
    using Localities = Array<hpx::id_type, rank>;


    /*!
        @brief      Class for representing partitioned arrays
        @tparam     Element Type for representing element values
        @tparam     rank Array rank

        The array is partitioned. Partitions can be located in multiple processes.
    */
    template<typename Element, Rank rank>
    class PartitionedArray
    {

        private:

            using PartitionClient = ArrayPartition<Element, rank>;

            using PartitionServer = typename PartitionClient::Server;

        public:

            using Partition = PartitionClient;

            using Partitions = ArrayPartitionData<Partition, rank>;

            using Count = typename Partitions::Count;

            using Offset = typename PartitionServer::Offset;

            using Shape = typename Partitions::Shape;

            using Iterator = typename Partitions::Iterator;

            using ConstIterator = typename Partitions::ConstIterator;

            PartitionedArray();

            PartitionedArray(PartitionedArray const& other) = delete;

            PartitionedArray(PartitionedArray&& other) = default;

            PartitionedArray(Shape const& shape, Localities<rank>&& localities, Partitions&& partitions);

            PartitionedArray(Shape const& shape, Localities<rank> const& localities, Partitions&& partitions);

            ~PartitionedArray() = default;

            PartitionedArray& operator=(PartitionedArray const& other) = delete;

            PartitionedArray& operator=(PartitionedArray&& other) = default;

            Count nr_elements() const;

            Shape const& shape() const;

            Localities<rank> const& localities() const;

            Count nr_partitions() const;

            Partitions& partitions();

            Partitions const& partitions() const;

            ConstIterator begin() const;

            Iterator begin();

            ConstIterator end() const;

            Iterator end();

        private:

            //! Shape of the partitioned array
            Shape _shape;

            //! Localities the partitions are located in
            Localities<rank> _localities;

            //! Array of partitions
            Partitions _partitions;

            void assert_invariants() const;
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
        @brief      Default-construct an instance with an empty shape

        The array will have zero elements.
    */
    template<typename Element, Rank rank>
    PartitionedArray<Element, rank>::PartitionedArray():

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


    /*!
        @brief      Construct an instance
        @param      shape Shape of the array
        @param      localities Localities where the partitions are located
        @param      partitions Collection of array partitions

        The shape of the partitions together must equal the shape passed in
    */
    template<typename Element, Rank rank>
    PartitionedArray<Element, rank>::PartitionedArray(
        Shape const& shape, Localities<rank>&& localities, Partitions&& partitions):

        _shape{shape},
        _localities{std::move(localities)},
        _partitions{std::move(partitions)}

    {
        assert_invariants();
    }


    /*!
        @brief      Construct an instance
        @param      shape Shape of the array
        @param      localities Localities where the partitions are located
        @param      partitions Collection of array partitions

        The shape of the partitions together must equal the shape passed in
    */
    template<typename Element, Rank rank>
    PartitionedArray<Element, rank>::PartitionedArray(
        Shape const& shape, Localities<rank> const& localities, Partitions&& partitions):

        _shape{shape},
        _localities{localities},
        _partitions{std::move(partitions)}

    {
        assert_invariants();
    }


    template<typename Element, Rank rank>
    void PartitionedArray<Element, rank>::assert_invariants() const
    {
        lue_hpx_assert(_partitions.shape() == _localities.shape());

        // The array is either empty, or all localities are valid / known
        lue_hpx_assert(
            _localities.empty() || std::all_of(
                                       _localities.begin(),
                                       _localities.end(),
                                       [](hpx::id_type const locality_id) { return bool{locality_id}; }));
    }


    /*!
        @brief      Return the number of elements
    */
    template<typename Element, Rank rank>
    typename PartitionedArray<Element, rank>::Count PartitionedArray<Element, rank>::nr_elements() const
    {
        return lue::nr_elements(_shape);
    }


    /*!
        @brief      Return the shape
    */
    template<typename Element, Rank rank>
    typename PartitionedArray<Element, rank>::Shape const& PartitionedArray<Element, rank>::shape() const
    {
        return _shape;
    }


    /*!
        @brief      Return the number of partitions
    */
    template<typename Element, Rank rank>
    typename PartitionedArray<Element, rank>::Count PartitionedArray<Element, rank>::nr_partitions() const
    {
        return _partitions.nr_elements();
    }


    /*!
        @brief      Return the partitions
    */
    template<typename Element, Rank rank>
    typename PartitionedArray<Element, rank>::Partitions& PartitionedArray<Element, rank>::partitions()
    {
        return _partitions;
    }


    /*!
        @brief      Return the partitions
    */
    template<typename Element, Rank rank>
    typename PartitionedArray<Element, rank>::Partitions const& PartitionedArray<Element, rank>::partitions()
        const
    {
        return _partitions;
    }


    /*!
        @brief      Return an iterator to the first partition
    */
    template<typename Element, Rank rank>
    typename PartitionedArray<Element, rank>::ConstIterator PartitionedArray<Element, rank>::begin() const
    {
        return _partitions.begin();
    }


    /*!
        @brief      Return an iterator to the first partition
    */
    template<typename Element, Rank rank>
    typename PartitionedArray<Element, rank>::Iterator PartitionedArray<Element, rank>::begin()
    {
        return _partitions.begin();
    }


    /*!
        @brief      Return an iterator to the one-past-the-last partition
    */
    template<typename Element, Rank rank>
    typename PartitionedArray<Element, rank>::ConstIterator PartitionedArray<Element, rank>::end() const
    {
        return _partitions.end();
    }


    /*!
        @brief      Return an iterator to the one-past-the-last partition
    */
    template<typename Element, Rank rank>
    typename PartitionedArray<Element, rank>::Iterator PartitionedArray<Element, rank>::end()
    {
        return _partitions.end();
    }


    /*!
        @brief      Return the localities
    */
    template<typename Element, Rank rank>
    Localities<rank> const& PartitionedArray<Element, rank>::localities() const
    {
        return _localities;
    }


    template<typename Element, Rank rank>
    static ShapeT<PartitionedArray<Element, rank>> shape(PartitionedArray<Element, rank> const& array)
    {
        return array.shape();
    }


    template<typename Element, Rank rank>
    static ShapeT<PartitionedArray<Element, rank>> shape_in_partitions(
        PartitionedArray<Element, rank> const& array)
    {
        return array.partitions().shape();
    }


    template<typename Element, Rank rank>
    static Count nr_partitions(PartitionedArray<Element, rank> const& array)
    {
        return array.nr_partitions();
    }


    template<typename Element, Rank rank>
    static Count nr_partitions(ArrayPartitionData<ArrayPartition<Element, rank>, rank> const& partitions)
    {
        return partitions.nr_elements();
    }


    template<typename Element, Rank rank>
    static typename PartitionedArray<Element, rank>::Count nr_elements(
        PartitionedArray<Element, rank> const& array)
    {
        return array.nr_elements();
    }


    template<typename Element, Rank rank>
    static std::string describe(Shape<Element, rank> const& shape)
    {
        // FIXME Requires fmt >= 6
        // return fmt::format("({})", shape);

        static_assert(rank > 0);

        auto it = shape.begin();
        std::string result = fmt::format("({}", *it);

        for (++it; it < shape.end(); ++it)
        {
            result += fmt::format(", {}", *it);
        }

        result += ")";

        return result;
    }


    // TODO Move this elsewhere
    template<typename Element, Rank rank>
    static std::string describe(PartitionedArray<Element, rank> const& array)
    {
        return fmt::format(
            "- PartitionedArray:\n"
            "    - array of elements  : {} ({} elements)\n"
            "    - array of partitions: {} ({} partitions)\n",
            describe(array.shape()),
            array.nr_elements(),
            describe(array.partitions().shape()),
            array.partitions().nr_elements());
    }

}  // namespace lue
