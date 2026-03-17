#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

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
        @brief      Return the number of elements
    */
    template<typename Element, Rank rank>
    auto PartitionedArray<Element, rank>::nr_elements() const -> Count
    {
        return lue::nr_elements(_shape);
    }


    /*!
        @brief      Return the shape
    */
    template<typename Element, Rank rank>
    auto PartitionedArray<Element, rank>::shape() const -> Shape const&
    {
        return _shape;
    }


    /*!
        @brief      Return the localities
    */
    template<typename Element, Rank rank>
    auto PartitionedArray<Element, rank>::localities() const -> Localities<rank> const&
    {
        return _localities;
    }


    /*!
        @brief      Return the number of partitions
    */
    template<typename Element, Rank rank>
    auto PartitionedArray<Element, rank>::nr_partitions() const -> Count
    {
        return _partitions.nr_elements();
    }


    /*!
        @brief      Return the partitions
    */
    template<typename Element, Rank rank>
    auto PartitionedArray<Element, rank>::partitions() -> Partitions&
    {
        return _partitions;
    }


    /*!
        @brief      Return the partitions
    */
    template<typename Element, Rank rank>
    auto PartitionedArray<Element, rank>::partitions() const -> Partitions const&
    {
        return _partitions;
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
                                       [](hpx::id_type const& locality_id) { return bool{locality_id}; }));
    }

}  // namespace lue
