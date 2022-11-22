#include "lue/framework/algorithm/value_policies/zonal_sum.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<
            typename Element,
            typename Zone,
            Rank rank>
        PartitionedArray<Element, rank> zonal_sum(
            PartitionedArray<Element, rank> const& array,
            PartitionedArray<Zone, rank> const& zones)
        {
            return value_policies::zonal_sum(array, zones);
        }

    }  // Anonymous namespace


    void bind_zonal_sum(
        pybind11::module& module)
    {
        module.def("zonal_sum", zonal_sum<std::uint8_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_sum", zonal_sum<std::uint8_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_sum", zonal_sum<std::uint8_t, std::uint64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_sum", zonal_sum<std::int32_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_sum", zonal_sum<std::int32_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_sum", zonal_sum<std::int32_t, std::uint64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_sum", zonal_sum<std::uint32_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_sum", zonal_sum<std::uint32_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_sum", zonal_sum<std::uint32_t, std::uint64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_sum", zonal_sum<std::int64_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_sum", zonal_sum<std::int64_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_sum", zonal_sum<std::int64_t, std::uint64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_sum", zonal_sum<std::uint64_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_sum", zonal_sum<std::uint64_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_sum", zonal_sum<std::uint64_t, std::uint64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_sum", zonal_sum<float, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_sum", zonal_sum<float, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_sum", zonal_sum<float, std::uint64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_sum", zonal_sum<double, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_sum", zonal_sum<double, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_sum", zonal_sum<double, std::uint64_t, 2>, "array"_a, "zones"_a);
    }

}  // namespace lue::framework
