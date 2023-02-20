#include "lue/framework/algorithm/value_policies/zonal_mean.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<typename Element, typename Zone, Rank rank>
        PartitionedArray<Element, rank> zonal_mean(
            PartitionedArray<Element, rank> const& array, PartitionedArray<Zone, rank> const& zones)
        {
            return value_policies::zonal_mean(array, zones);
        }

    }  // Anonymous namespace


    void bind_zonal_mean(pybind11::module& module)
    {
        module.def("zonal_mean", zonal_mean<float, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_mean", zonal_mean<float, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_mean", zonal_mean<float, std::uint64_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_mean", zonal_mean<float, std::int32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_mean", zonal_mean<float, std::int64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_mean", zonal_mean<double, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_mean", zonal_mean<double, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_mean", zonal_mean<double, std::uint64_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_mean", zonal_mean<double, std::int32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_mean", zonal_mean<double, std::int64_t, 2>, "array"_a, "zones"_a);
    }

}  // namespace lue::framework
