#include "lue/framework/serial_route_decl.hpp"


namespace lue {

    /*!
        @brief      Default-construct an instance
    */
    template<typename RouteID, Rank rank>
    SerialRoute<RouteID, rank>::SerialRoute():

        _array_shape{},
        _starts{},
        _partitions{}

    {
        // Shape is filled with indeterminate values! This may or may not
        // matter, depending on what happens next with this instance. To be sure,
        // explicitly set the shape to empty.
        _array_shape.fill(0);

        assert_invariants();
    }


    /*!
        @brief      Construct an instance, given an array @a shape, a shared future to the
                    route @a starts, and @a partitions

        The array shape passed in must match the union of array partitions.
    */
    template<typename RouteID, Rank rank>
    SerialRoute<RouteID, rank>::SerialRoute(
        Shape const& shape, hpx::shared_future<Starts>&& starts, Partitions&& partitions):

        _array_shape{shape},
        _starts{std::move(starts)},
        _partitions{std::move(partitions)}

    {
        assert_invariants();
    }


    /*!
        @brief      Construct an instance, given an array @a shape, a unique future to the
                    route @a starts, and @a partitions

        The array shape passed in must match the union of array partitions.
    */
    template<typename RouteID, Rank rank>
    SerialRoute<RouteID, rank>::SerialRoute(
        Shape const& shape, hpx::future<Starts>&& starts, Partitions&& partitions):

        SerialRoute{shape, starts.share(), std::move(partitions)}

    {
        assert_invariants();
    }


    /*!
        @brief      Return the shape of the array
    */
    template<typename RouteID, Rank rank>
    auto SerialRoute<RouteID, rank>::shape() const -> Shape const&
    {
        return _array_shape;
    }


    /*!
        @brief      Return a shared future to the collection of route starts
    */
    template<typename RouteID, Rank rank>
    auto SerialRoute<RouteID, rank>::starts() const -> hpx::shared_future<Starts> const&
    {
        lue_hpx_assert(valid());

        return _starts;
    }


    /*!
        @brief      Return the collection of route partitions
    */
    template<typename RouteID, Rank rank>
    auto SerialRoute<RouteID, rank>::partitions() const -> Partitions const&
    {
        return _partitions;
    }


    template<typename RouteID, Rank rank>
    auto SerialRoute<RouteID, rank>::nr_routes() const -> Count
    {
        lue_hpx_assert(valid());
        lue_hpx_assert(is_ready());

        return static_cast<Count>(std::size(starts().get()));
    }


    template<typename RouteID, Rank rank>
    auto SerialRoute<RouteID, rank>::valid() const -> bool
    {
        return _starts.valid();
    }


    template<typename RouteID, Rank rank>
    auto SerialRoute<RouteID, rank>::is_ready() const -> bool
    {
        return _starts.is_ready();
    }


    template<typename RouteID, Rank rank>
    void SerialRoute<RouteID, rank>::assert_invariants() const
    {
        // The array is either empty, or all partitions are valid (don't need to be ready)
        lue_hpx_assert(
            (std::all_of(
                 _array_shape.begin(), _array_shape.end(), [](auto const extent) { return extent == 0; }) &&
             nr_elements(_partitions.shape()) == 0) ||
            (std::all_of(
                 _array_shape.begin(), _array_shape.end(), [](auto const extent) { return extent > 0; }) &&
             std::all_of(
                 _partitions.begin(),
                 _partitions.end(),
                 [](auto const partition) { return partition.valid(); })));
    }

}  // namespace lue
