#include "lue/framework/algorithm/value_policies/zonal_minimum.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<
            typename Element,
            typename Zone,
            Rank rank>
        PartitionedArray<Element, rank> zonal_minimum(
            PartitionedArray<Element, rank> const& array,
            PartitionedArray<Zone, rank> const& zones)
        {
            return value_policies::zonal_minimum(array, zones);
        }

    }  // Anonymous namespace


    void bind_zonal_minimum(
        pybind11::module& module)
    {
        module.def("zonal_minimum", zonal_minimum<std::uint8_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::uint8_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::uint8_t, std::uint64_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::uint8_t, std::int32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::uint8_t, std::int64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_minimum", zonal_minimum<std::int32_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::int32_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::int32_t, std::uint64_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::int32_t, std::int32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::int32_t, std::int64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_minimum", zonal_minimum<std::uint32_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::uint32_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::uint32_t, std::uint64_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::uint32_t, std::int32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::uint32_t, std::int64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_minimum", zonal_minimum<std::int64_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::int64_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::int64_t, std::uint64_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::int64_t, std::int32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::int64_t, std::int64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_minimum", zonal_minimum<std::uint64_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::uint64_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::uint64_t, std::uint64_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::uint64_t, std::int32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<std::uint64_t, std::int64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_minimum", zonal_minimum<float, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<float, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<float, std::uint64_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<float, std::int32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<float, std::int64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_minimum", zonal_minimum<double, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<double, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<double, std::uint64_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<double, std::int32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_minimum", zonal_minimum<double, std::int64_t, 2>, "array"_a, "zones"_a);
    }

}  // namespace lue::framework
