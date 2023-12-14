#include "lue/framework/algorithm/value_policies/highest_n.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    namespace detail {

        template<typename OutputElement, Rank rank>
        auto highest_n(SerialRoute<OutputElement, rank> const& route, Count const max_nr_cells)
        {
            return lue::value_policies::highest_n<OutputElement>(route, max_nr_cells);
        }


        template<typename OutputElement, typename ZoneElement, typename ValueElement, Rank rank>
        auto highest_n_zonal(
            PartitionedArray<ZoneElement, rank> const& zones,
            PartitionedArray<ValueElement, rank> const& values,
            Count const max_nr_cells)
        {
            return lue::value_policies::highest_n<OutputElement>(zones, values, max_nr_cells);
        }


        template<typename OutputElement, typename ValueElement, Rank rank>
        auto highest_n_global(PartitionedArray<ValueElement, rank> const& values, Count const max_nr_cells)
        {
            return lue::value_policies::highest_n<OutputElement>(values, max_nr_cells);
        }


        template<typename ZoneElement, typename ValueElement>
        void bind_highest_n(pybind11::module& module)
        {
            using OutputElement = std::uint8_t;
            Rank const rank{2};

            module.def("highest_n", highest_n<OutputElement, rank>);
            module.def("highest_n", highest_n_global<OutputElement, ValueElement, rank>);
            module.def("highest_n", highest_n_zonal<OutputElement, ZoneElement, ValueElement, rank>);
        }

    }  // namespace detail

    void bind_highest_n(pybind11::module& module)
    {
        detail::bind_highest_n<std::uint8_t, std::uint8_t>(module);
        detail::bind_highest_n<std::int32_t, std::uint8_t>(module);
        detail::bind_highest_n<std::int64_t, std::uint8_t>(module);
        detail::bind_highest_n<std::uint32_t, std::uint8_t>(module);
        detail::bind_highest_n<std::uint64_t, std::uint8_t>(module);

        detail::bind_highest_n<std::uint8_t, std::int32_t>(module);
        detail::bind_highest_n<std::int32_t, std::int32_t>(module);
        detail::bind_highest_n<std::int64_t, std::int32_t>(module);
        detail::bind_highest_n<std::uint32_t, std::int32_t>(module);
        detail::bind_highest_n<std::uint64_t, std::int32_t>(module);

        detail::bind_highest_n<std::uint8_t, std::int64_t>(module);
        detail::bind_highest_n<std::int32_t, std::int64_t>(module);
        detail::bind_highest_n<std::int64_t, std::int64_t>(module);
        detail::bind_highest_n<std::uint32_t, std::int64_t>(module);
        detail::bind_highest_n<std::uint64_t, std::int64_t>(module);

        detail::bind_highest_n<std::uint8_t, std::uint32_t>(module);
        detail::bind_highest_n<std::int32_t, std::uint32_t>(module);
        detail::bind_highest_n<std::int64_t, std::uint32_t>(module);
        detail::bind_highest_n<std::uint32_t, std::uint32_t>(module);
        detail::bind_highest_n<std::uint64_t, std::uint32_t>(module);

        detail::bind_highest_n<std::uint8_t, std::uint64_t>(module);
        detail::bind_highest_n<std::int32_t, std::uint64_t>(module);
        detail::bind_highest_n<std::int64_t, std::uint64_t>(module);
        detail::bind_highest_n<std::uint32_t, std::uint64_t>(module);
        detail::bind_highest_n<std::uint64_t, std::uint64_t>(module);

        detail::bind_highest_n<std::uint8_t, float>(module);
        detail::bind_highest_n<std::int32_t, float>(module);
        detail::bind_highest_n<std::int64_t, float>(module);
        detail::bind_highest_n<std::uint32_t, float>(module);
        detail::bind_highest_n<std::uint64_t, float>(module);

        detail::bind_highest_n<std::uint8_t, double>(module);
        detail::bind_highest_n<std::int32_t, double>(module);
        detail::bind_highest_n<std::int64_t, double>(module);
        detail::bind_highest_n<std::uint32_t, double>(module);
        detail::bind_highest_n<std::uint64_t, double>(module);
    }

}  // namespace lue::framework
