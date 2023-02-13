#include "lue/framework/algorithm/value_policies/zonal_area.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<
            typename Count,
            typename Zone,
            Rank rank>
        PartitionedArray<Count, rank> zonal_area(
            PartitionedArray<Zone, rank> const& zones)
        {
            return value_policies::zonal_area<Count>(zones);
        }

    }  // Anonymous namespace


    void bind_zonal_area(
        pybind11::module& module)
    {
        module.def("zonal_area", zonal_area<std::uint64_t, std::uint8_t, 2>, "zones"_a);
        module.def("zonal_area", zonal_area<std::uint64_t, std::uint32_t, 2>, "zones"_a);
        module.def("zonal_area", zonal_area<std::uint64_t, std::uint64_t, 2>, "zones"_a);
        module.def("zonal_area", zonal_area<std::uint64_t, std::int32_t, 2>, "zones"_a);
        module.def("zonal_area", zonal_area<std::uint64_t, std::int64_t, 2>, "zones"_a);
    }

}  // namespace lue::framework
