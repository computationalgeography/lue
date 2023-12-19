#include "lue/framework/algorithm/value_policies/highest_n.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace detail {

        template<typename RouteID, Rank rank>
        auto highest_n(SerialRoute<RouteID, rank> const& route, Count const max_nr_cells)
            -> PartitionedArray<RouteID, rank>
        {
            // TODO Make the output element type configurable?
            using OutputElement = RouteID;

            return lue::value_policies::highest_n<OutputElement>(route, max_nr_cells);
        }


        template<typename RouteID, Rank rank>
        auto bind_highest_n(pybind11::module& module)
        {
            module.def("highest_n", highest_n<RouteID, rank>);
        }

    }  // namespace detail


    void bind_highest_n(pybind11::module& module)
    {
        Rank const rank{2};

        detail::bind_highest_n<std::uint8_t, rank>(module);
        detail::bind_highest_n<std::uint32_t, rank>(module);
        detail::bind_highest_n<std::uint64_t, rank>(module);
        detail::bind_highest_n<std::int32_t, rank>(module);
        detail::bind_highest_n<std::int64_t, rank>(module);
    }

}  // namespace lue::framework
