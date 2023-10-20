#pragma once
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework/serial_route.hpp"


namespace lue {
    namespace detail {

    }  // namespace detail


    template<typename Policies, typename Functor, typename... Result, Rank rank>
    void walk(Policies const& policies, SerialRoute<rank> const& route, Functor&& functor, Result&... results)
    {
        // Apply a copy of the functor passed in to each cell of each route

        // How to create and fill the result? Each functor instance can write to corresponding
        // cells in the output, in parallel.

        // The walk call can be part of an operation that creates the functor and the results
        // and calls walk with all this information.

        // functor.apply(locality, etc, ...results);
    }

}  // namespace lue
